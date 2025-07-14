// ============================================================================
// MAIN - Choose between beacon or listener mode
// ============================================================================
int main(int argc, char* argv[]) {
    std::cout << "🏰 SIMPLE LIGHTHOUSE BEACON/LISTENER BOTS\n";
    std::cout << "════════════════════════════════════════════\n\n";
    
    try {
        if (argc > 1 && std::string(argv[1]) == "beacon") {
            SimpleBeaconBot beacon;
            beacon.run();
        } else if (argc > 1 && std::string(argv[1]) == "listener") {
            int port = 9876;
            if (argc > 2) {
                port = std::atoi(argv[2]);
            }
            SimpleListenerBot listener(port);
            listener.run();
        } else {
            std::cout << "Usage:\n";
            std::cout << "  " << argv[0] << " beacon     - Start beacon bot\n";
            std::cout << "  " << argv[0] << " listener [port] - Start listener bot\n\n";
            
            std::cout << "Choose mode:\n";
            std::cout << "1. Beacon Bot (sends lighthouse signals)\n";
            std::cout << "2. Listener Bot (receives lighthouse signals)\n";
            std::cout << "Choice (1/2): ";
            
            int choice;
            std::cin >> choice;
            
            if (choice == 1) {
                SimpleBeaconBot beacon;
                beacon.run();
            } else if (choice == 2) {
                SimpleListenerBot listener;
                listener.run();
            } else {
                std::cout << "Invalid choice.\n";
                return 1;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
v