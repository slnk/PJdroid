#include "jni-include/common.h"

#define THIS_FILE "APP"

/* Callback called by the library upon receiving incoming call */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                             pjsip_rx_data *rdata) {
    pjsua_call_info ci;

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &ci);

    PJ_LOG(3, (THIS_FILE, "Incoming call from %.*s!!",
            (int) ci.remote_info.slen,
            ci.remote_info.ptr));

    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer(call_id, 200, NULL, NULL);
}

/* Callback called by the library when call's state has changed */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
    pjsua_call_info ci;

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(3, (THIS_FILE, "Call %d state=%.*s", call_id,
            (int) ci.state_text.slen,
            ci.state_text.ptr));
}

/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id) {
    pjsua_call_info ci;

    pjsua_call_get_info(call_id, &ci);

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        // When media is active, connect call to sound device.
        pjsua_conf_connect(ci.conf_slot, 0);
        pjsua_conf_connect(0, ci.conf_slot);
    }
}

/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status) {
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
    exit(1);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_spectralink_pjdroid_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    pjsua_acc_id acc_id;
    pj_status_t status;
    std::string my_status = "OK";

    /* Create pjsua first! */
    status = pjsua_create();
    if (status != PJ_SUCCESS) {
        pjsua_perror("native-lib.cpp", "Error in pjsua_create()", status);
        my_status = "Error in pjsua_create()";
        return env->NewStringUTF(my_status.c_str());
    }

    /* If argument is specified, it's got to be a valid SIP URL */
    status = pjsua_verify_url("sip:VH3@sip-10001.accounts.qos.vocal-dev.com");
    if (status != PJ_SUCCESS) {
        pjsua_perror("native-lib.cpp", "Error in pjsua_verify_url()", status);
        my_status = "Error in pjsua_verify_url()";
        return env->NewStringUTF(my_status.c_str());
    }

    /* Init pjsua */
    {
        pjsua_config cfg;
        pjsua_logging_config log_cfg;

        pjsua_config_default(&cfg);
        cfg.cb.on_incoming_call = &on_incoming_call;
        cfg.cb.on_call_media_state = &on_call_media_state;
        cfg.cb.on_call_state = &on_call_state;

        pjsua_logging_config_default(&log_cfg);
        log_cfg.console_level = 4;

        status = pjsua_init(&cfg, &log_cfg, NULL);
        if (status != PJ_SUCCESS) {
            pjsua_perror("native-lib.cpp", "Error in pjsua_init()", status);
            my_status = "Error in pjsua_init()";
            return env->NewStringUTF(my_status.c_str());
        }
    }

    /* Add UDP transport. */
    {
        pjsua_transport_config cfg;

        pjsua_transport_config_default(&cfg);
        cfg.port = 5080;
        status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
        if (status != PJ_SUCCESS) {
            pjsua_perror("native-lib.cpp", "Error in pjsua_transport_create()", status);
            pjsua_destroy();
            my_status = "Error in pjsua_transport_create()";
            return env->NewStringUTF(my_status.c_str());
        }
    }

    /* Initialization is done, now start pjsua */
    status = pjsua_start();
    if (status != PJ_SUCCESS) {
        pjsua_destroy();
        pjsua_perror("native-lib.cpp", "Error in pjsua_start()", status);
        return env->NewStringUTF(my_status.c_str());
    }

    /* Register to SIP server by creating SIP account. */
    {
        pjsua_acc_config cfg;

        pjsua_acc_config_default(&cfg);
        cfg.id = pj_str("sip:VH_AC1_2@a10001.ac1.accounts.devvpc.vocal-dev.com");
        cfg.reg_uri = pj_str("sip:a10001.ac1.accounts.devvpc.vocal-dev.com");
        cfg.cred_count = 1;
        cfg.cred_info[0].realm = pj_str("a10001.ac1.accounts.devvpc.vocal-dev.com");
        cfg.cred_info[0].scheme = pj_str("digest");
        cfg.cred_info[0].username = pj_str("VH_AC1_2");
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg.cred_info[0].data = pj_str("123");

        status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
        if (status != PJ_SUCCESS) {
            pjsua_perror("native-lib.cpp", "Error in adding account", status);
            pjsua_destroy();
            my_status = "Error in adding account";
            return env->NewStringUTF(my_status.c_str());
        }
    }

    /* If URL is specified, make call to the URL. */
    //pj_str_t uri = pj_str("VH3@sip-10001.accounts.qos.vocal-dev.com");
    //status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
    //if (status != PJ_SUCCESS) error_exit("Error making call", status);
//
//    /* Wait until user press "q" to quit. */
//    for (;;) {
//        char option[10];
//
//        puts("Press 'h' to hangup all calls, 'q' to quit");
//        if (fgets(option, sizeof(option), stdin) == NULL) {
//            puts("EOF while reading stdin, will quit now..");
//            break;
//        }
//
//        if (option[0] == 'q')
//            break;
//
//        if (option[0] == 'h')
//            pjsua_call_hangup_all();
//    }
//
//    /* Destroy pjsua */
//    status = pjsua_destroy();
//    if (status != PJ_SUCCESS) {
//        pjsua_perror("native-lib.cpp", "Error in pjsua_destroy()", status);
//        return env->NewStringUTF(my_status.c_str());
//    }
//    my_status = "pjsua_destroy() successfully";
//
    return env->NewStringUTF(my_status.c_str());
}
