var metadata = {
    name: "Network-BOF",
    description: "Network tools: port scanning, connectivity checks"
};

var portscan = ax.create_command("portscan", "Scan ports on a target", "portscan <target> <port> [timeout]");
portscan.addArgString("target", false, "Target IP address");
portscan.addArgInt("port", false, "Starting port number");
portscan.addArgInt("timeout", true, "Timeout in ms (default 2000)");
portscan.setPreHook(function(id, cmdline, parsed) {
    let target = parsed[0] || "127.0.0.1";
    let port = parseInt(parsed[1]) || 80;
    let timeout = parseInt(parsed[2]) || 2000;
    let params = ax.bof_pack("int,str,int,int", [0, target, port, timeout]);
    let bof = ax.script_dir() + "_bin/portscan." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Port Scan");
});

var portscan_range = ax.create_command("portscan-range", "Scan port range on a target", "portscan-range <target> <start> <end>");
portscan_range.addArgString("target", false, "Target IP address");
portscan_range.addArgInt("start", false, "Start port");
portscan_range.addArgInt("end", false, "End port");
portscan_range.setPreHook(function(id, cmdline, parsed) {
    let target = parsed[0] || "127.0.0.1";
    let start = parseInt(parsed[1]) || 1;
    let end = parseInt(parsed[2]) || 1000;
    let params = ax.bof_pack("int,str,int,int", [1, target, start, end]);
    let bof = ax.script_dir() + "_bin/portscan." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Port Range Scan");
});

var group = ax.create_commands_group("Network", [portscan, portscan_range]);
ax.register_commands_group(group, ["beacon", "beacon9090"], ["windows"], []);
