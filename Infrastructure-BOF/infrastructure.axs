var metadata = {
    name: "Infrastructure-BOF",
    description: "Network infrastructure: SCADA/ICS, network devices, IoT"
};

var scada_info = ax.create_command("scada-info", "Show SCADA/ICS protocol information", "scada-info");
scada_info.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [2]);
    let bof = ax.script_dir() + "_bin/network_device." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: SCADA/ICS Info");
});

var iot_info = ax.create_command("iot-info", "IoT device detection and defaults", "iot-info");
iot_info.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [3]);
    let bof = ax.script_dir() + "_bin/network_device." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: IoT Info");
});

var group = ax.create_commands_group("Infrastructure", [scada_info, iot_info]);
ax.register_commands_group(group, ["beacon", "beacon9090"], ["windows"], []);
