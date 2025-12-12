// Command Table for text-based protocol
const W5500::W5500_Command W5500::command_table[] = {
    {"PING",   &W5500::handlePingCommand,   "PING - Test connectivity"},
    {"STATUS", &W5500::handleStatusCommand, "STATUS - Get device status"},
    {"RESET",  &W5500::handleResetCommand,  "RESET - Reset the W5500 chip"},
    {"HELP",   &W5500::handleHelpCommand,   "HELP - Show available commands"}
    // Extensible: Add more commands here
};

const int W5500::num_commands = 
    sizeof(W5500::command_table) / sizeof(W5500::W5500_Command);
