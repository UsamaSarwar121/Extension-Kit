var metadata = {
    name: "Mobile-BOF",
    description: "Mobile device access: Android ADB, iOS, network devices, IoT"
};

var android_adb = ax.create_command("mobile-android-adb", "Read Android ADB keys", "mobile-android-adb");
android_adb.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [0]);
    let bof = ax.script_dir() + "_bin/mobile." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Android ADB Keys");
});

var mobile_deploy = ax.create_command("mobile-deploy", "Show mobile agent deployment methods", "mobile-deploy");
mobile_deploy.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [1]);
    let bof = ax.script_dir() + "_bin/mobile." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Mobile Deployment");
});

var netdevice = ax.create_command("netdevice-info", "Network device access commands", "netdevice-info");
netdevice.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [2]);
    let bof = ax.script_dir() + "_bin/mobile." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Network Device Info");
});

var iot_detect = ax.create_command("iot-detect", "IoT device detection and defaults", "iot-detect");
iot_detect.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [3]);
    let bof = ax.script_dir() + "_bin/mobile." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: IoT Detection");
});

var group = ax.create_commands_group("Mobile & IoT", [android_adb, mobile_deploy, netdevice, iot_detect]);
ax.register_commands_group(group, ["beacon", "beacon9090"], ["windows"], []);
