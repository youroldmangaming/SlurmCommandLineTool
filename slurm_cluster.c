#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define MAX_COMMAND_LENGTH 256
#define MAX_SRUN_COMMAND_LENGTH 1024
#define MAX_OUTPUT_LENGTH 1024
#define MAX_EXCLUDE_LENGTH 256
#define MAX_NODELIST_LENGTH 1024

int main(int argc, char *argv[]) {
    char exclude[MAX_EXCLUDE_LENGTH] = "";
    int opt;

    // Parse command line options
    while ((opt = getopt(argc, argv, "e:")) != -1) {
        switch (opt) {
            case 'e':
                strncpy(exclude, optarg, MAX_EXCLUDE_LENGTH - 1);
                exclude[MAX_EXCLUDE_LENGTH - 1] = '\0';
                break;
            default:
                fprintf(stderr, "Usage: %s [-e exclude_list] <command>\n", argv[0]);
                return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected command after options\n");
        return 1;
    }

    // Concatenate remaining arguments into a single command string
    char command[MAX_COMMAND_LENGTH] = "";
    for (int i = optind; i < argc; i++) {
        if (strlen(command) + strlen(argv[i]) + 2 > MAX_COMMAND_LENGTH) {
            fprintf(stderr, "Command too long\n");
            return 1;
        }
        strcat(command, argv[i]);
        if (i < argc - 1) strcat(command, " ");
    }

    // Get the total number of nodes in the cluster
    FILE *node_count_fp = popen("sinfo -h -N | wc -l", "r");
    if (node_count_fp == NULL) {
        perror("Failed to get node count");
        return 1;
    }
    int total_nodes;
    fscanf(node_count_fp, "%d", &total_nodes);
    pclose(node_count_fp);

    // Get the list of all nodes
    char all_nodes[MAX_NODELIST_LENGTH] = "";
    FILE *nodelist_fp = popen("sinfo -h -N -o %N", "r");
    if (nodelist_fp == NULL) {
        perror("Failed to get node list");
        return 1;
    }
    fgets(all_nodes, sizeof(all_nodes), nodelist_fp);
    pclose(nodelist_fp);

    // Count the number of excluded nodes
    int excluded_count = 0;
    if (strlen(exclude) > 0) {
        excluded_count = 1;  // Start at 1 for the first comma
        for (char *p = exclude; *p; p++) {
            if (*p == ',') excluded_count++;
        }
    }

    // Calculate available nodes
    int available_nodes = total_nodes - excluded_count;

    if (available_nodes <= 0) {
        fprintf(stderr, "Error: No nodes available after exclusion\n");
        return 1;
    }

    // Format the Slurm srun command
    char srun_command[MAX_SRUN_COMMAND_LENGTH];
    if (strlen(exclude) > 0) {
        snprintf(srun_command, sizeof(srun_command), 
                 "srun --nodes=%d --ntasks-per-node=1 --exclude=%s bash -c 'echo \"$(hostname): $(%s)\"' 2>&1", 
                 available_nodes, exclude, command);
    } else {
        snprintf(srun_command, sizeof(srun_command), 
                 "srun --nodes=%d --ntasks-per-node=1 bash -c 'echo \"$(hostname): $(%s)\"' 2>&1", 
                 total_nodes, command);
    }

    // Run the srun command and capture output
    FILE *fp = popen(srun_command, "r");
    if (fp == NULL) {
        perror("Failed to run command");
        return 1;
    }

    // Print the output from each node
    char output[MAX_OUTPUT_LENGTH];
    int error_occurred = 0;
    while (fgets(output, sizeof(output) - 1, fp) != NULL) {
        if (strstr(output, "srun: error:") != NULL) {
            fprintf(stderr, "Error: %s", output);
            error_occurred = 1;
        } else {
            printf("%s", output);
        }
    }

    int status = pclose(fp);
    if (status != 0 || error_occurred) {
        fprintf(stderr, "Command failed with status %d\n", status);
        fprintf(stderr, "Possible reasons for failure:\n");
        fprintf(stderr, "1. Insufficient resources available\n");
        fprintf(stderr, "2. Excluded all available nodes\n");
        fprintf(stderr, "3. Permission issues\n");
        fprintf(stderr, "Please check your Slurm configuration and try again.\n");
        return 1;
    }

    return 0;
}









