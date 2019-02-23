#include "UsbmuxdConnector.hpp"
#include <memory>
int main(int argc, char *argv[]) {
    std::shared_ptr<socketkit::UsbmuxdConnector> connector = std::make_shared<socketkit::UsbmuxdConnector>();
    connector->getRunloop()->run();
    std::this_thread::sleep_for(std::chrono::seconds(100));
    return 0;
}