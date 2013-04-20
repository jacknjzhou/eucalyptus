// -*- mode: C; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil -*-
// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

/*************************************************************************
 * Copyright 2009-2012 Eucalyptus Systems, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 *
 * Please contact Eucalyptus Systems, Inc., 6755 Hollister Ave., Goleta
 * CA 93117, USA or visit http://www.eucalyptus.com/licenses/ if you need
 * additional information or have any questions.
 *
 * This file may incorporate work covered under the following copyright
 * and permission notice:
 *
 *   Software License Agreement (BSD License)
 *
 *   Copyright (c) 2008, Regents of the University of California
 *   All rights reserved.
 *
 *   Redistribution and use of this software in source and binary forms,
 *   with or without modification, are permitted provided that the
 *   following conditions are met:
 *
 *     Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *     Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer
 *     in the documentation and/or other materials provided with the
 *     distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *   COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE. USERS OF THIS SOFTWARE ACKNOWLEDGE
 *   THE POSSIBLE PRESENCE OF OTHER OPEN SOURCE LICENSED MATERIAL,
 *   COPYRIGHTED MATERIAL OR PATENTED MATERIAL IN THIS SOFTWARE,
 *   AND IF ANY SUCH MATERIAL IS DISCOVERED THE PARTY DISCOVERING
 *   IT MAY INFORM DR. RICH WOLSKI AT THE UNIVERSITY OF CALIFORNIA,
 *   SANTA BARBARA WHO WILL THEN ASCERTAIN THE MOST APPROPRIATE REMEDY,
 *   WHICH IN THE REGENTS' DISCRETION MAY INCLUDE, WITHOUT LIMITATION,
 *   REPLACEMENT OF THE CODE SO IDENTIFIED, LICENSING OF THE CODE SO
 *   IDENTIFIED, OR WITHDRAWAL OF THE CODE CAPABILITY TO THE EXTENT
 *   NEEDED TO COMPLY WITH ANY SUCH LICENSES OR RIGHTS.
 ************************************************************************/

//!
//! @file node/NCclient.c
//! Need to provide description
//!

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                                  INCLUDES                                  |
 |                                                                            |
\*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>                    /* getopt */

#include <data.h>

#include "client-marshal.h"

#include <misc.h>
#include <euca_axis.h>
#include <sensor.h>
#include <eucalyptus.h>
#include <adb-helpers.h>
#include <euca_string.h>

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                                  DEFINES                                   |
 |                                                                            |
\*----------------------------------------------------------------------------*/

#define NC_ENDPOINT                 "/axis2/services/EucalyptusNC"
#define WALRUS_ENDPOINT             "/services/Walrus"
#define DEFAULT_WALRUS_HOSTPORT     "localhost:8773"
#define DEFAULT_NC_HOSTPORT         "localhost:8775"
#define DEFAULT_MAC_ADDR            "aa:bb:cc:dd:ee:ff"
#define DEFAULT_PUBLIC_IP           "10.1.2.3"
#define BUFSIZE                     1024

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                                  TYPEDEFS                                  |
 |                                                                            |
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                                ENUMERATIONS                                |
 |                                                                            |
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                                 STRUCTURES                                 |
 |                                                                            |
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                             EXTERNAL VARIABLES                             |
 |                                                                            |
\*----------------------------------------------------------------------------*/

/* Should preferably be handled in header file */

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                              GLOBAL VARIABLES                              |
 |                                                                            |
\*----------------------------------------------------------------------------*/

boolean debug = FALSE;                 //!< Enables debug mode if set to TRUE

#ifndef NO_COMP
const char *euca_this_component_name = "nc";    //!< Eucalyptus Component Name
const char *euca_client_component_name = "user";    //!< The client component name
#endif /* ! NO_COMP */

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                              STATIC VARIABLES                              |
 |                                                                            |
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                              STATIC PROTOTYPES                             |
 |                                                                            |
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                                   MACROS                                   |
 |                                                                            |
\*----------------------------------------------------------------------------*/

#define CHECK_PARAM(par, name) if (par==NULL) { fprintf (stderr, "ERROR: no %s specified (try -h)\n", name); exit (1); }

/*----------------------------------------------------------------------------*\
 |                                                                            |
 |                               IMPLEMENTATION                               |
 |                                                                            |
\*----------------------------------------------------------------------------*/

//!
//! Prints the command help to stderr
//!
void usage(void)
{
    fprintf(stderr, "usage: NCclient [command] [options]\n"
            "\tcommands:\t\t\trequired options:\n"
            "\t\trunInstance\t\t[-m -k] or multiple [-v]\n"
            "\t\tterminateInstance\t[-i]\n"
            "\t\tdescribeInstances\n"
            "\t\tdescribeResource\n"
            "\t\tattachVolume\t\t[-i -V -R -L]\n"
            "\t\tdetachVolume\t\t[-i -V -R -L]\n"
            "\t\tbundleInstance\t\t[-i]\n"
            "\t\tbundleRestartInstance\t[-i]\n"
            "\t\tdescribeSensors\n"
            "\t\tmodifyNode\t\t[-s]\n"
            "\t\tmigrateInstances\t\t[-i -M]\n"
            "\toptions:\n"
            "\t\t-d \t\t- print debug output\n"
            "\t\t-l \t\t- local invocation => do not use WSSEC\n"
            "\t\t-h \t\t- this help information\n"
            "\t\t-w [host:port] \t- Walrus endpoint\n"
            "\t\t-n [host:port] \t- NC endpoint\n"
            "\t\t-B -n node-ip \t- one of nodes controled by VB\n"
            "\t\t-i [str] \t- instance ID\n"
            "\t\t-e [str] \t- reservation ID\n"
            "\t\t-v [type:id:size:format:guestDeviceName:resourceLocation]\n"
            "\t\t\ttype = {machine|kernel|ramdisk|ephemeral|ebs}\n"
            "\t\t\tid = {none|emi-...|eki-...|eri-...|vol-...}\n"
            "\t\t\tsize = {-1|NNNN} - in bytes, only for local partitions\n"
            "\t\t\tformat = {none|ext3|swap} - only for local partitions\n"
            "\t\t\tguestDeviceName = {none|x?[vhsf]d[a-z]?[1-9]*} - e.g., sda1\n"
            "\t\t\tresourceLocation = {none|walrus://...|iqn://...|aoe://...}\n"
            "\t\t-m [id:path] \t- id and manifest path of disk image\n"
            "\t\t-k [id:path] \t- id and manifest path of kernel image\n"
            "\t\t-r [id:path] \t- id and manifest path of ramdisk image\n"
            "\t\t-a [address] \t- MAC address for instance to use\n"
            "\t\t-c [number] \t- number of instances to start\n"
            "\t\t-V [name] \t- name of the volume (for reference)\n"
            "\t\t-R [device] \t- remote/source device (e.g. /dev/etherd/e0.0)\n"
            "\t\t-L [device] \t- local/target device (e.g. hda)\n"
            "\t\t-F \t\t- force VolumeDetach\n"
            "\t\t-U [string] \t- user data to store with instance\n" "\t\t-I [string] \t- launch index to store with instance\n"
            "\t\t-G [str:str: ] \t- group names to store with instance\n"
            "\t\t-s [stateName] \t- name of state\n"
            "\t\t\t\tUse {enabled|disabled} for modifyNode operation\n"
            "\t\t\t\tUse {prepare|commit|rollback} for migrateInstances opration\n" "\t\t-M [src:dst:cr]\t- migration request source and destination IPs + credentials\n");

    exit(1);
}

//!
//! parse spec_str (-v parameter) into a VBR record and add it to the
//! vm_type->virtualBootRecord[virtualBootRecordLen]
//!
//! @param[in] spec_str
//! @param[in] vm_type
//!
//! @return EUCA_OK on success or EUCA_ERROR on failure
//!
int add_vbr(const char *spec_str, virtualMachine * vm_type)
{
    if (vm_type->virtualBootRecordLen == EUCA_MAX_VBRS) {
        fprintf(stderr, "ERROR: too many -v parameters\n");
        return EUCA_ERROR;
    }
    virtualBootRecord *vbr = &(vm_type->virtualBootRecord[vm_type->virtualBootRecordLen++]);

    char *spec_copy = strdup(spec_str);
    char *type_spec = strtok(spec_copy, ":");
    char *id_spec = strtok(NULL, ":");
    char *size_spec = strtok(NULL, ":");
    char *format_spec = strtok(NULL, ":");
    char *dev_spec = strtok(NULL, ":");
    char *loc_spec = strtok(NULL, ":");
    if (type_spec == NULL) {
        fprintf(stderr, "ERROR: invalid 'type' specification in VBR '%s'\n", spec_str);
        goto out_error;
    }
    euca_strncpy(vbr->typeName, type_spec, sizeof(vbr->typeName));
    if (id_spec == NULL) {
        fprintf(stderr, "ERROR: invalid 'id' specification in VBR '%s'\n", spec_str);
        goto out_error;
    }
    euca_strncpy(vbr->id, id_spec, sizeof(vbr->id));
    if (size_spec == NULL) {
        fprintf(stderr, "ERROR: invalid 'size' specification in VBR '%s'\n", spec_str);
        goto out_error;
    }
    vbr->sizeBytes = atoi(size_spec);
    if (format_spec == NULL) {
        fprintf(stderr, "ERROR: invalid 'format' specification in VBR '%s'\n", spec_str);
        goto out_error;
    }
    euca_strncpy(vbr->formatName, format_spec, sizeof(vbr->formatName));
    if (dev_spec == NULL) {
        fprintf(stderr, "ERROR: invalid 'guestDeviceName' specification in VBR '%s'\n", spec_str);
        goto out_error;
    }
    euca_strncpy(vbr->guestDeviceName, dev_spec, sizeof(vbr->guestDeviceName));
    if (loc_spec == NULL) {
        fprintf(stderr, "ERROR: invalid 'resourceLocation' specification in VBR '%s'\n", spec_str);
        goto out_error;
    }
    euca_strncpy(vbr->resourceLocation, spec_str + (loc_spec - spec_copy), sizeof(vbr->resourceLocation));

    EUCA_FREE(spec_copy);
    return EUCA_OK;

out_error:
    vm_type->virtualBootRecordLen--;
    EUCA_FREE(spec_copy);
    return EUCA_ERROR;
}

//!
//! Main entry point of the application
//!
//! @param[in] argc the number of parameter passed on the command line
//! @param[in] argv the list of arguments
//!
//! @return Always return 0 or exit(1) on failure
//!
int main(int argc, char **argv)
{
    ncMetadata meta = { "correlate-me-please", "eucalyptus" };
    virtualMachine params = { 64, 1, 1, "m1.small", NULL, NULL, NULL, NULL, NULL, NULL, {}, 0 };
    char *nc_hostport = DEFAULT_NC_HOSTPORT;
    char *walrus_hostport = DEFAULT_WALRUS_HOSTPORT;
    char *nc_endpoint = NC_ENDPOINT;
    char *instance_id = NULL;
    char *image_id = NULL;
    char *image_manifest = NULL;
    char *kernel_id = NULL;
    char *kernel_manifest = NULL;
    char *ramdisk_id = NULL;
    char *ramdisk_manifest = NULL;
    char *reservation_id = NULL;
    char *uu_id = NULL;
    char *mac_addr = strdup(DEFAULT_MAC_ADDR);
    char *public_ip = strdup(DEFAULT_PUBLIC_IP);
    char *volume_id = NULL;
    char *remote_dev = NULL;
    char *local_dev = NULL;
    int force = 0;
    char *user_data = NULL;
    char *launch_index = NULL;
    char **group_names = NULL;
    int group_names_size = 0;
    char *state_name = NULL;
    char *src_node_name = NULL;
    char *dst_node_name = NULL;
    char *migration_creds = NULL;
    char *timestamp_str = NULL;
    char *command = NULL;
    int local = 0;
    int count = 1;
    int ch = 0;
    int rc = 0;

    while ((ch = getopt(argc, argv, "lhdn:w:i:m:k:r:e:a:c:h:u:p:V:R:L:FU:I:G:v:t:s:M:B")) != -1) {
        switch (ch) {
        case 'c':
            count = atoi(optarg);
            break;
        case 'd':
            debug = 1;
            break;
        case 'l':
            local = 1;
            break;
        case 'n':
            nc_hostport = optarg;
            break;
        case 'w':
            walrus_hostport = optarg;
            break;
        case 'i':
            instance_id = optarg;
            break;
        case 'p':
            public_ip = optarg;
            break;
        case 'm':
            image_id = strtok(optarg, ":");
            image_manifest = strtok(NULL, ":");
            if (image_id == NULL || image_manifest == NULL) {
                fprintf(stderr, "ERROR: could not parse image [id:manifest] paramters (try -h)\n");
                exit(1);
            }
            break;
        case 'k':
            kernel_id = strtok(optarg, ":");
            kernel_manifest = strtok(NULL, ":");
            if (kernel_id == NULL || kernel_manifest == NULL) {
                fprintf(stderr, "ERROR: could not parse kernel [id:manifest] paramters (try -h)\n");
                exit(1);
            }
            break;
        case 'r':
            ramdisk_id = strtok(optarg, ":");
            ramdisk_manifest = strtok(NULL, ":");
            if (ramdisk_id == NULL || ramdisk_manifest == NULL) {
                fprintf(stderr, "ERROR: could not parse ramdisk [id:manifest] paramters (try -h)\n");
                exit(1);
            }
            break;
        case 'e':
            reservation_id = optarg;
            break;
        case 'u':
            uu_id = optarg;
            break;
        case 'a':
            mac_addr = optarg;
            break;
        case 'V':
            volume_id = optarg;
            break;
        case 'R':
            remote_dev = optarg;
            break;
        case 'L':
            local_dev = optarg;
            break;
        case 'F':
            force = 1;
            break;
        case 'U':
            user_data = optarg;
            break;
        case 'I':
            launch_index = optarg;
            break;
        case 'G':
            {
                int i;
                group_names_size = 1;
                for (i = 0; optarg[i]; i++)
                    if (optarg[i] == ':')
                        group_names_size++;
                group_names = EUCA_ZALLOC(group_names_size, sizeof(char *));
                if (group_names == NULL) {
                    fprintf(stderr, "ERROR: out of memory for group_names[]\n");
                    exit(1);
                }
                group_names[0] = strtok(optarg, ":");
                for (i = 1; i < group_names_size; i++)
                    group_names[i] = strtok(NULL, ":");
                break;
            }
        case 's':
            state_name = optarg;
            break;
        case 'M':
            src_node_name = strtok(optarg, ":");
            dst_node_name = strtok(NULL, ":");
            migration_creds = strtok(NULL, ":");
            if (src_node_name == NULL || dst_node_name == NULL) {
                fprintf(stderr, "ERROR: could not parse migration [src:dst:cr] paramters (try -h)\n");
                exit(1);
            }
            break;
        case 'v':
            if (add_vbr(optarg, &params)) {
                fprintf(stderr, "ERROR: could not parse the virtual boot record (try -h)\n");
                exit(1);
            }
            break;
        case 't':
            timestamp_str = optarg;
            break;
        case 'h':
            usage();                   // will exit
            break;
        case 'B':
            nc_endpoint = "/services/EucalyptusBroker";
            break;
        case '?':
        default:
            fprintf(stderr, "ERROR: unknown parameter (try -h)\n");
            exit(1);
        }
    }
    argc -= optind;
    argv += optind;

    if (argc > 0) {
        command = argv[0];
        if (argc > 1) {
            fprintf(stderr, "WARNING: too many parameters, using first one as command\n");
        }
    } else {
        fprintf(stderr, "ERROR: command not specified (try -h)\n");
        exit(1);
    }

    ncStub *stub;
    char configFile[1024], policyFile[1024];
    char *euca_home;
    int use_wssec;
    char *tmpstr;

    euca_home = getenv("EUCALYPTUS");
    if (!euca_home) {
        euca_home = "";
    }
    snprintf(configFile, 1024, EUCALYPTUS_CONF_LOCATION, euca_home);
    snprintf(policyFile, 1024, EUCALYPTUS_KEYS_DIR "/nc-client-policy.xml", euca_home);
    rc = get_conf_var(configFile, "ENABLE_WS_SECURITY", &tmpstr);
    if (rc != 1) {
/* Default to enabled */
        use_wssec = 1;
    } else {
        if (!strcmp(tmpstr, "Y")) {
            use_wssec = 1;
        } else {
            use_wssec = 0;
        }
    }

    char nc_url[BUFSIZE];
    snprintf(nc_url, BUFSIZE, "http://%s%s", nc_hostport, nc_endpoint);
    if (debug)
        printf("connecting to NC at %s\n", nc_url);
    stub = ncStubCreate(nc_url, "NCclient.log", NULL);
    if (!stub) {
        fprintf(stderr, "ERROR: failed to connect to Web service\n");
        exit(2);
    }

    char walrus_url[BUFSIZE];
    snprintf(walrus_url, BUFSIZE, "http://%s%s", walrus_hostport, WALRUS_ENDPOINT);
    serviceInfoType *si = &(meta.services[meta.servicesLen++]);
    euca_strncpy(si->type, "walrus", sizeof(si->type));
    euca_strncpy(si->name, "walrus", sizeof(si->name));
    euca_strncpy(si->uris[0], walrus_url, sizeof(si->uris[0]));
    si->urisLen = 1;

    if (use_wssec && !local) {
        if (debug)
            printf("using policy file %s\n", policyFile);
        rc = InitWSSEC(stub->env, stub->stub, policyFile);
        if (rc) {
            fprintf(stderr, "ERROR: cannot initialize WS-SEC policy from %s\n", policyFile);
            exit(1);
        }
    }

    char *image_url = NULL;
    if (image_manifest) {
        char t[BUFSIZE];
        snprintf(t, BUFSIZE, "http://%s%s/%s", walrus_hostport, WALRUS_ENDPOINT, image_manifest);
        image_url = strdup(t);
    }

    char *kernel_url = NULL;
    if (kernel_manifest) {
        char t[BUFSIZE];
        snprintf(t, BUFSIZE, "http://%s%s/%s", walrus_hostport, WALRUS_ENDPOINT, kernel_manifest);
        kernel_url = strdup(t);
    }

    char *ramdisk_url = NULL;
    if (ramdisk_manifest) {
        char t[BUFSIZE];
        snprintf(t, BUFSIZE, "http://%s%s/%s", walrus_hostport, WALRUS_ENDPOINT, ramdisk_manifest);
        ramdisk_url = strdup(t);
    }

    /***********************************************************/
    if (!strcmp(command, "runInstance")) {
        if (params.virtualBootRecordLen < 1) {
            CHECK_PARAM(image_id, "image ID and manifest path");
            CHECK_PARAM(kernel_id, "kernel ID and manifest path");
        }

        char *privMac, *privIp;
        char *platform = NULL;
        int vlan = 3;
        privMac = strdup(mac_addr);
        mac_addr[0] = 'b';
        mac_addr[1] = 'b';
        privIp = strdup("10.0.0.202");
        srand(time(NULL));

/* generate random IDs if they weren't specified */
#define C rand()%26 + 97

        while (count--) {
            char *iid, *rid, *uuid;

            char ibuf[8];
            if (instance_id == NULL || count > 1) {
                snprintf(ibuf, 8, "i-%c%c%c%c%c", C, C, C, C, C);
                iid = ibuf;
            } else {
                iid = instance_id;
            }

            char rbuf[8];
            if (reservation_id == NULL || count > 1) {
                snprintf(rbuf, 8, "r-%c%c%c%c%c", C, C, C, C, C);
                rid = rbuf;
            } else {
                rid = reservation_id;
            }

            char ubuf[48];
            if (uu_id == NULL || count > 1) {
                snprintf(ubuf, 48, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C,
                         C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C);
                uuid = ubuf;
            } else {
                uuid = uu_id;
            }

            netConfig netparams;
            ncInstance *outInst;
            bzero(&netparams, sizeof(netparams));
            netparams.vlan = vlan;
            snprintf(netparams.privateIp, 24, "%s", privIp);
            snprintf(netparams.privateMac, 24, "%s", privMac);

            rc = ncRunInstanceStub(stub, &meta, uuid, iid, rid, &params, image_id, image_url, kernel_id, kernel_url, ramdisk_id, ramdisk_url, "eucalyptusUser", "eucalyptusAccount", "",    /* key */
                                   &netparams,
                                   //                                       privMac, privIp, vlan,
                                   user_data, launch_index, platform, 0, group_names, group_names_size, /* CC stuff */
                                   &outInst);
            if (rc != 0) {
                printf("ncRunInstance() failed: instanceId=%s error=%d\n", instance_id, rc);
                exit(1);
            }
// count device mappings
            int i, count = 0;
            for (i = 0; i < EUCA_MAX_VBRS; i++) {
                if (strlen(outInst->params.virtualBootRecord[i].typeName) > 0)
                    count++;
            }
            printf("instanceId=%s stateCode=%d stateName=%s deviceMappings=%d/%d\n", outInst->instanceId, outInst->stateCode, outInst->stateName,
                   count, outInst->params.virtualBootRecordLen);
        }

    /***********************************************************/
    } else if (!strcmp(command, "bundleInstance")) {
        CHECK_PARAM(instance_id, "instance id");
        rc = ncBundleInstanceStub(stub, &meta, instance_id, "bucket-foo", "prefix-foo", "s3-url-foo", "user-key-foo", "s3policy-foo", "s3policy-sig");
        printf("ncBundleInstanceStub = %d\n", rc);

    } else if (!strcmp(command, "bundleRestartInstance")) {
        CHECK_PARAM(instance_id, "instance id");
        rc = ncBundleRestartInstanceStub(stub, &meta, instance_id);
        printf("ncBundleRestartInstanceStub = %d\n", rc);
    } else if (!strcmp(command, "powerDown")) {
        rc = ncPowerDownStub(stub, &meta);
    } else if (!strcmp(command, "describeBundleTasks")) {
        char *instIds[4];
        int instIdsLen;
        instIds[0] = EUCA_ZALLOC(32, sizeof(char));
        instIds[1] = EUCA_ZALLOC(32, sizeof(char));
        instIds[2] = EUCA_ZALLOC(32, sizeof(char));
        instIds[3] = EUCA_ZALLOC(32, sizeof(char));
        snprintf(instIds[0], 32, "i-12345675");
        snprintf(instIds[1], 32, "i-12345674");
        snprintf(instIds[2], 32, "i-12345673");
        snprintf(instIds[3], 32, "i-12345672");
        instIdsLen = 4;
        bundleTask **outBundleTasks = NULL;
        int outBundleTasksLen = 0;
        rc = ncDescribeBundleTasksStub(stub, &meta, instIds, instIdsLen, &outBundleTasks, &outBundleTasksLen);
        for (int i = 0; i < outBundleTasksLen; i++) {
            printf("BUNDLE %d: %s %s\n", i, outBundleTasks[i]->instanceId, outBundleTasks[i]->state);
        }
    } else if (!strcmp(command, "assignAddress")) {
        rc = ncAssignAddressStub(stub, &meta, instance_id, public_ip);
    } else if (!strcmp(command, "terminateInstance")) {
        CHECK_PARAM(instance_id, "instance ID");

        int shutdownState, previousState;
        rc = ncTerminateInstanceStub(stub, &meta, instance_id, 0, &shutdownState, &previousState);
        if (rc != 0) {
            printf("ncTerminateInstance() failed: error=%d\n", rc);
            exit(1);
        }
        printf("shutdownState=%d, previousState=%d\n", shutdownState, previousState);

    /***********************************************************/
    } else if (!strcmp(command, "describeInstances")) {
        //! @TODO pull out of argv[] requested instanceIDs */
        ncInstance **outInsts;
        int outInstsLen, i;
        rc = ncDescribeInstancesStub(stub, &meta, NULL, 0, &outInsts, &outInstsLen);
        if (rc != 0) {
            printf("ncDescribeInstances() failed: error=%d\n", rc);
            exit(1);
        }
        for (i = 0; i < outInstsLen; i++) {
            ncInstance *inst = outInsts[i];
            printf("instanceId=%s state=%s time=%d\n", inst->instanceId, inst->stateName, inst->launchTime);
            if (debug) {
                printf("              userData=%s launchIndex=%s groupNames=", inst->userData, inst->launchIndex);
                if (inst->groupNamesSize > 0) {
                    int j;
                    for (j = 0; j < inst->groupNamesSize; j++) {
                        if (j > 0)
                            printf(":");
                        printf("%s", inst->groupNames[j]);
                    }
                } else {
                    printf("(none)");
                }
                printf("\n");

                printf("              attached volumes: ");
                int vol_count = 0;
                for (int j = 0; j < EUCA_MAX_VOLUMES; j++) {
                    if (strlen(inst->volumes[j].volumeId) > 0) {
                        if (vol_count > 0)
                            printf("                                ");
                        printf("%s %s %s\n", inst->volumes[j].volumeId, inst->volumes[j].attachmentToken, inst->volumes[j].localDev);
                    }
                }
                if (vol_count)
                    printf("(none)\n");

                free_instance(&(outInsts[i]));
            }
        }
        //! @TODO: fix free(outInsts);

    /***********************************************************/
    } else if (!strcmp(command, "describeResource")) {
        char *type = NULL;
        ncResource *outRes;

        rc = ncDescribeResourceStub(stub, &meta, type, &outRes);
        if (rc != 0) {
            printf("ncDescribeResource() failed: error=%d\n", rc);
            exit(1);
        }
        printf("node status=[%s] memory=%d/%d disk=%d/%d cores=%d/%d subnets=[%s]\n",
               outRes->nodeStatus,
               outRes->memorySizeMax, outRes->memorySizeAvailable, outRes->diskSizeMax, outRes->diskSizeAvailable, outRes->numberOfCoresMax,
               outRes->numberOfCoresAvailable, outRes->publicSubnets);

    /***********************************************************/
    } else if (!strcmp(command, "attachVolume")) {
        CHECK_PARAM(instance_id, "instance ID");
        CHECK_PARAM(volume_id, "volume ID");
        CHECK_PARAM(remote_dev, "remote dev");
        CHECK_PARAM(local_dev, "local dev");

        rc = ncAttachVolumeStub(stub, &meta, instance_id, volume_id, remote_dev, local_dev);
        if (rc != 0) {
            printf("ncAttachVolume() failed: error=%d\n", rc);
            exit(1);
        }

    /***********************************************************/
    } else if (!strcmp(command, "detachVolume")) {
        CHECK_PARAM(instance_id, "instance ID");
        CHECK_PARAM(volume_id, "volume ID");
        CHECK_PARAM(remote_dev, "remote dev");
        CHECK_PARAM(local_dev, "local dev");

        rc = ncDetachVolumeStub(stub, &meta, instance_id, volume_id, remote_dev, local_dev, force);
        if (rc != 0) {
            printf("ncDetachVolume() failed: error=%d\n", rc);
            exit(1);
        }

    /***********************************************************/
    } else if (!strcmp(command, "describeSensors")) {

        sensorResource **res;
        int resSize;

        rc = ncDescribeSensorsStub(stub, &meta, 20, 5000, NULL, 0, NULL, 0, &res, &resSize);
        if (rc != 0) {
            printf("ncDescribeSensors() failed: error=%d\n", rc);
            exit(1);
        }
        char buf[102400];
        sensor_res2str(buf, sizeof(buf), res, resSize);
        printf("resources: %d\n%s\n", resSize, buf);

    /***********************************************************/
    } else if (!strcmp(command, "modifyNode")) {
        CHECK_PARAM(state_name, "state name");

        rc = ncModifyNodeStub(stub, &meta, state_name);
        if (rc != 0) {
            printf("ncModifyNode() failed: error=%d\n", rc);
            exit(1);
        }

    /***********************************************************/
    } else if (!strcmp(command, "migrateInstances")) {
        CHECK_PARAM(instance_id, "instance ID");
        CHECK_PARAM(src_node_name, "source node name");
        CHECK_PARAM(dst_node_name, "destination node name");
        CHECK_PARAM(state_name, "state name");
        // migration creds can be NULL

        ncInstance instance;
        bzero(&instance, sizeof(ncInstance));
        ncInstance *instances = &instance;
        strncpy(instance.instanceId, instance_id, sizeof(instance.instanceId));
        strncpy(instance.migration_src, src_node_name, sizeof(instance.migration_src));
        strncpy(instance.migration_dst, dst_node_name, sizeof(instance.migration_dst));
        rc = ncMigrateInstancesStub(stub, &meta, &instances, 1, state_name, migration_creds);
        if (rc != 0) {
            printf("ncMigrateInstances() failed: error=%d\n", rc);
            exit(1);
        }

    /***********************************************************/
    } else if (!strcmp(command, "_convertTimestamp")) {
        CHECK_PARAM(timestamp_str, "timestamp");
        long long ts_in = atoll(timestamp_str);

        axutil_date_time_t *dt = unixms_to_datetime(stub->env, ts_in);
        char *dt_in = axutil_date_time_serialize_date_time(dt, stub->env);

        long long ts_out = datetime_to_unixms(dt, stub->env);
        printf("timestamp:  in = %lld %s\n", ts_in, dt_in);
        printf("           out = %lld\n", ts_out);

    /***********************************************************/
    } else {
        fprintf(stderr, "ERROR: command %s unknown (try -h)\n", command);
        exit(1);
    }

    if (local) {
        pthread_exit(NULL);
    } else {
        _exit(0);
    }
}
