var metadata = {
    name: "Container-BOF",
    description: "Container enumeration: Docker, Kubernetes, Podman, WSL"
};

var docker_conf = ax.create_command("container-docker-config", "Read Docker Desktop config", "container-docker-config");
docker_conf.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [0]);
    let bof = ax.script_dir() + "_bin/container." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Docker Config");
});

var k8s_config = ax.create_command("container-k8s-config", "Read Kubernetes config", "container-k8s-config");
k8s_config.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [1]);
    let bof = ax.script_dir() + "_bin/container." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Kubernetes Config");
});

var container_detect = ax.create_command("container-detect", "Detect container environments", "container-detect");
container_detect.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [2]);
    let bof = ax.script_dir() + "_bin/container." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Container Detection");
});

var container_cmds = ax.create_command("container-commands", "Show container enumeration commands", "container-commands");
container_cmds.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [3]);
    let bof = ax.script_dir() + "_bin/container." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Container Commands");
});

var group = ax.create_commands_group("Containers", [docker_conf, k8s_config, container_detect, container_cmds]);
ax.register_commands_group(group, ["beacon", "beacon9090"], ["windows"], []);
