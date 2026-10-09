var metadata = {
    name: "Satellite-BOF",
    description: "Satellite/VSAT: modem detection, ground station, ICS/SCADA protocols"
};

var vsat_scan = ax.create_command("vsat-scan", "Scan VSAT modem interfaces", "vsat-scan");
vsat_scan.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int,str", [0, ""]);
    let bof = ax.script_dir() + "_bin/satellite." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: VSAT Scan");
});

var vsat_detect = ax.create_command("vsat-detect", "Detect satellite/VSAT infrastructure", "vsat-detect");
vsat_detect.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int,str", [1, ""]);
    let bof = ax.script_dir() + "_bin/satellite." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: VSAT Detection");
});

var ics_info = ax.create_command("ics-info", "Show ICS/SCADA protocol information", "ics-info");
ics_info.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int,str", [2, ""]);
    let bof = ax.script_dir() + "_bin/satellite." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: ICS/SCADA Info");
});

var ground_station = ax.create_command("ground-station", "Satellite ground station info", "ground-station");
ground_station.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int,str", [3, ""]);
    let bof = ax.script_dir() + "_bin/satellite." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Ground Station Info");
});

var group = ax.create_commands_group("Satellite", [vsat_scan, vsat_detect, ics_info, ground_station]);
ax.register_commands_group(group, ["beacon", "beacon9090"], ["windows"], []);
