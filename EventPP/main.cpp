//
//  main.cpp
//  EventPP
//
//  Created by CmST0us on 2017/9/4.
//  Copyright © 2017年 CmST0us. All rights reserved.
//
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <string>
#include "SocketAddress.hpp"
#include "SocketException.hpp"
#include "TCPServer.hpp"
#include "TCPConnection.hpp"
#include "EchoProtocolSyntaxAdapter.hpp"
#include "ConsoleEchoProtocolSyntaxAdapter.hpp"
#include "CustomSyntaxAdapter.hpp"
#include "EvBufferAdapter.hpp"
#include "TCPConnectionFactory.hpp"
#include "Socks5ProtocolSyntaxAdapter.hpp"
#include "UDPServer.hpp"
using namespace ts;
using namespace std;
void test_SocketAddress() {
    try {
        auto start = std::chrono::system_clock::now();
        
        auto address = ts::SocketAddress("www.baidu.com", 80);
        address.waitForResolveFinish();
        auto end = std::chrono::system_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout<<double(dur.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
        //    std::this_thread::sleep_for(std::chrono::seconds(1));
        if (address.getIpString().length() == 0) {
            std::cout<<"Error"<<address.getErrorMessage()<<std::endl;
        } else {
            std::cout<<"ip address is:"<<address.getIpString()<<std::endl;
        }
    } catch (SocketException e) {
        std::cout<<e.getExceptionDescription();
    }
}
    
void test_TCPServer_ConsoleEcho() {
    ts::TCPServer server(12000);
    server.setup();
    auto syntaxer = std::shared_ptr<ts::ConsoleEchoProtocolSyntaxAdapter>(new ts::ConsoleEchoProtocolSyntaxAdapter);
    server.setProtocolSyntax(syntaxer);
    
//    std::thread([&](){
        server.start();
//    }).detach();
    
}

void test_TCPConnection() {
    auto connection = new ts::TCPConnection("www.baidu.com", 80);
    connection->connect();
    auto syntaxer = std::shared_ptr<ts::EchoProtocolSyntaxAdapter>(new ts::EchoProtocolSyntaxAdapter);
    connection->setProtocolSyntax(syntaxer);
//    std::thread([&](){
        connection->start();
//    });
}

void test_TCPConnection_CustomHandle() {
    auto connection = new ts::TCPConnection("www.baidu.com", 80);
    connection->connect();
    auto syntaxer = std::shared_ptr<ts::CustomSyntaxAdapter>(new ts::CustomSyntaxAdapter([&](ts::ProtocolSyntax::EventType type, ts::InputStream *i, ts::OutputStream *o, void *ctx){
        switch (type) {
            case ts::ProtocolSyntax::Connect: {
                std::string sss("GET index.html HTTP/1.1\r\n\r\n");
                o->write(sss.c_str(), sss.length());
            }
                break;
            case ts::ProtocolSyntax::Read: {
                int l = i->length();
                std::unique_ptr<unsigned char[]> buf(new unsigned char[l]);
                memset(buf.get(), 0, l);
                i->read(buf.get(), l);
                printf("%s", buf.get());
            }
                break;
            default:
                break;
        }
    }));
    connection->setProtocolSyntax(syntaxer);
    connection->start();
}
//
enum Socks5Status {
    Auth = 4,
    Connect = 5,
};
std::map<std::string, Socks5Status> clientStatus;
std::map<std::string, TCPConnection *>remoteConnections;

void test_Sock5ToSSServer() {
    auto sockServer = new ts::TCPServer(12000);
    auto syntax = [&](ProtocolSyntax::EventType type, InputStream* inputStream, OutputStream *outputStream, void *ctx){
        auto connection = (TCPConnection *)ctx;
        
        switch (type) {
            case ProtocolSyntax::Read: {
                switch (clientStatus[connection->getSocketAddress().ipPortPairString()]) {
                    case Auth: {
                        int len = inputStream->length();
                        cout<<"req len after auth: "<<len<<endl;
                        if (len < 5) {
                            break;
                        }
                        unsigned char buf[1024] = {0};
                        inputStream->fetch(buf, 5);
                        int aType = buf[3];
                        int reqLen = 0;
                        if (aType == 0x01) {
                            reqLen = 10;
                        } else if (aType == 0x03) {
                            reqLen = 5 + buf[4] + 2;
                        } else if (aType == 0x04) {
                            reqLen = 22;
                        }
                        if (len < reqLen) {
                            break;
                        }
                        
                        inputStream->read(buf, reqLen);
                        for (int a = 0; a < reqLen; ++a) {
                            printf("%02X ", buf[a]);
                        }
                        printf("\n");
                        
                        int sockCommand = buf[1];
                        aType = buf[3];
                        uint32_t destAddr = 0;
                        uint16_t destPort = 0;
                        string destAddrString;
                        string destPortString;
                        if (aType == 0x01) {
                            destAddr = *((uint32_t *)(buf + 4));
                            destPort = *((uint16_t *)(buf + 4 + 8));
                            destPort = ntohs(destPort);
                            
                            struct in_addr inAddr;
                            inAddr.s_addr = destAddr;
                            destAddrString = inet_ntoa(inAddr);
                            stringstream sss;
                            sss<<destPort;
                            destPortString = sss.str();
                            
                        } else if (aType == 0x03) {
                            int destLen = buf[4];
                            stringstream ss;
                            stringstream sss;
                            for (int p = 0; p < destLen; ++p) {
                                ss<<buf[5 + p];
                            }
                            
                            destAddrString = ss.str();
                            uint8_t h = *((uint8_t *)(buf + 5 + destLen));
                            uint8_t l = *((uint8_t *)(buf + 5 + destLen + 1));
                            
                            destPort = h;
                            destPort = (destPort << 8) | l;
                            
                            sss<<destPort;
                            destPortString = sss.str();
                        } else if (aType == 0x04) {
                            //unsupport
                        }
                        cout<<"dest host: "<<destAddrString<<":"<<destPortString<<endl;
                        
                        string key = connection->getSocketAddress().ipPortPairString();
                        auto destConnection = new TCPConnection(destAddrString, destPortString);
                        remoteConnections[key] = destConnection;
                        auto destTrasmitHandler = [&](ProtocolSyntax::EventType type, InputStream* input, OutputStream *output, void *ctx){
                            //   (触发)
                            //R -> P -> L
                            int inl = input->length();
                            if (inl > 0) {
                                unsigned char *inpbuf = new unsigned char[inl];
                                memset(inpbuf, 0, inl);
                                input->read(inpbuf, inl);
#warning libevent 检测链路状态！
                                outputStream->write(inpbuf, inl);
                                delete [] inpbuf;
                                return ;
                            }
                        };
                        
                        auto destTransmitSyn = shared_ptr<ProtocolSyntax>(new CustomSyntaxAdapter(destTrasmitHandler));
                        destConnection->setProtocolSyntax(destTransmitSyn);
                        destConnection->connect();
                        std::thread([&](){
                            destConnection->start();
                        }).detach();
                        
                        
                        int bindPort = 12000;
                        short nBindPort = htons(bindPort);
                        string bindIp = "127.0.0.1";
                        
                        cout<<"bind Port: "<<bindPort<<"; bind Ip"<<bindIp<<"\n";
                        memset(buf, 0, 1024);
                        buf[0] = 0x05;
                        buf[1] = 0x00;
                        buf[2] = 0x00;
                        buf[3] = 0x03;
                        int bindIpLen = strlen("0.0.0.0");
                        buf[4] = bindIpLen;
                        strcpy((char *)buf + 5, "0.0.0.0");
                        memcpy(buf + 5 + bindIpLen, &nBindPort, 2);
                        int ll = 5 + bindIpLen + 2;
                        outputStream->write(buf, ll);
                        clientStatus[connection->getSocketAddress().ipPortPairString()] = Connect;
                    }
                        break;
                    case Connect: {
                        //   (触发)
                        // L -> P -> R
                        string key = connection->getSocketAddress().ipPortPairString();
                        auto remoteConn = remoteConnections[key];
                        int rdl = inputStream->length();
                        if (rdl > 0) {
                            unsigned char *rdbuf = new unsigned char[rdl];
                            memset(rdbuf, 0, rdl);
                            inputStream->read(rdbuf, rdl);
                            
                            if (remoteConn != nullptr) {
                                remoteConn->getOutputStream()->write(rdbuf, rdl);
                            }
                            
                            delete [] rdbuf;
                            return ;
                        }
#warning 通知关闭链路
                    }
                        break;
                    default: {
                        int len = inputStream->length();
                        if (len < 3) {
                            break;
                        }
                        unsigned char buf[1024] = {0};
                        inputStream->fetch(buf, 2);
                        if (buf[0] != 0x05) {
                            //version unsupport
                        }
                        int methodLen = buf[1];
                        int reqLen = 2 + methodLen;
                        if (len < reqLen) {
                            break;
                        }
                        inputStream->read(buf, reqLen);
                        int supportAuthFlag = 0;
                        for (int i = 0; i < buf[1]; ++i) {
                            if (buf[2 + i] == 0) {
                                supportAuthFlag = 1;
                            }
                        }
                        if (!supportAuthFlag) {
                            break;
                        }
                        unsigned char res[2] = {0x05, 0x00};
                        outputStream->write(res, 2);
                        clientStatus[connection->getSocketAddress().ipPortPairString()] = Auth;
                    }
                        break;
                }
            }
                break;
            case ProtocolSyntax::Accept: {
                
            }
                break;
            default:
                break;
        }
    };
    auto adapter = std::shared_ptr<CustomSyntaxAdapter>(new CustomSyntaxAdapter(syntax));
    sockServer->setup();
    sockServer->setProtocolSyntax(adapter);
    sockServer->start();
    
    delete sockServer;
}
void test_TCPServer_randomPort() {
    auto server = new TCPServer();
    server->setup();
    cout<<server->getSocketAddress().getPort();
    server->start();
}

using namespace std;
void test_TCPServer_eof() {
    auto ser = new TCPServer(12300);
    auto handler = [&](ProtocolSyntax::EventType type, InputStream* inputStream, OutputStream *outputStream, void *ctx){
        
        auto readLen = inputStream->length();
        auto buf = new char[readLen];
        memset(buf, 0, readLen);
        inputStream->read(buf, readLen);
        
        for (int i = 0; i < readLen; ++i) {
            printf("%c", buf[i]);
        }
        delete [] buf;
        
    };
    
    auto syntax = shared_ptr<ProtocolSyntax>(new CustomSyntaxAdapter(handler));
    ser->setProtocolSyntax(syntax);
    ser->setup();
    thread([&](){
        ser->start();
    }).detach();
    
    while (1) {
        string strIn = "[Server]: Hearbeat";
//        this_thread::sleep_for(chrono::milliseconds(10));
//        cin>>strIn;
        strIn = strIn + "\n";
//        ser->test_Boardcast((void*)strIn.c_str(), strIn.length());
        
    }
    
}

void test_socket_state() {
    auto server = new TCPServer(12121);
    auto syn = shared_ptr<ProtocolSyntax>(new CustomSyntaxAdapter());
    server->setProtocolSyntax(syn);
    server->setup();
    thread([&](){
        server->start();
    }).detach();
    this_thread::sleep_for(chrono::seconds(5));
    server->stop();
    delete server;
    this_thread::sleep_for(chrono::seconds(5));
}
void test_ShortLink() {
    for (int p = 0; p < 100; ++p) {
        EvBufferAdapter sndBuf;
        sndBuf.init();
        
        string s = "GET index.html HTTP/1.1\r\n\r\n";
        sndBuf.write(s.c_str(), s.length());
        
        
        auto conection = TCPConnectionFactory::shortLinkTCPConnection(sndBuf);
        conection->connect("www.baidu.com", 80);
        conection->start();
        
        char buf[1024] = {0};
        int len = sndBuf.length();
        sndBuf.read(buf, len);
        for (int i = 0; i < len; ++i) {
            printf("%c", buf[i]);
        }
        delete conection;
        sndBuf.free();
    }
    while(1);
}
void test_LongLink() {
    auto longConnection = TCPConnectionFactory::longLinkTCPConnection();
    auto customSyntax = (CustomSyntaxAdapter *)longConnection->getProtocolSyntax();
    customSyntax->mOnEOFEventHandler = [&](void *ctx) {
        auto ctxConnection = (TCPConnection *)ctx;
        ctxConnection->close();
    };
    
    longConnection->connect("127.0.0.1", 12003);
    
    thread([&](){
        longConnection->start();
        printf("over");
    }).detach();
    while (1) {
        longConnection->getOutputStream()->write("Hello\n", 6);
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void test_Socks5Proxy() {
    auto socks5ProxyServer = new TCPServer(12000);
    auto syntax = std::shared_ptr<Socks5ProtocolSyntaxAdapter>(new Socks5ProtocolSyntaxAdapter);
    socks5ProxyServer->setProtocolSyntax(syntax);
    socks5ProxyServer->setup();
    socks5ProxyServer->start();
}

void test_multiEventBase() {
    auto server = new TCPServer(12002);
    server->setProtocolSyntax(std::shared_ptr<ProtocolSyntax>(new EchoProtocolSyntaxAdapter));
    server->setup();
    server->start();
}
void signal_handle(int f) {
    
}


void test_UDPServer() {
    auto server = new UDPServer(12001);
    server->setup();
    server->start();
}
int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
     signal(SIGPIPE, SIG_IGN);
//    test_SocketAddress();
//    test_TCPServer_ConsoleEcho();
//    test_TCPConnection();
//    test_TCPConnection_CustomHandle();
//    test_Sock5ToSSServer();  //[TODO] Stream share pointer fix
//    test_TCPServer_randomPort();
//    test_TCPServer_eof();
//    test_socket_state();
//    test_ShortLink();
//    test_LongLink();
    test_Socks5Proxy();
//    test_multiEventBase();
//    test_UDPServer();
//
    return 0;
}


