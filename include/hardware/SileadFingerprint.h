#ifndef _AREMOTEAPI_HARDWARE_FINGERPRINT_H_
#define _AREMOTEAPI_HARDWARE_FINGERPRINT_H_

#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <cutils/log.h>
#include <hardware/hardware.h>
#include <hardware/fingerprint.h>

#include "ainffpsvcclient.h"
#include "ainffpsvcfpapkrelayerCB.h"

class SileadFingerprint{
    public:
        uint64_t fingerprint_pre_enroll(struct fingerprint_device __unused *dev);
        int fingerprint_post_enroll(struct fingerprint_device __unused *dev);
        int fingerprint_enroll(struct fingerprint_device __unused *dev,
                                const hw_auth_token_t __unused *hat,
                                uint32_t __unused gid,
                                uint32_t __unused timeout_sec);
        uint64_t fingerprint_get_auth_id(struct fingerprint_device __unused *dev);
        int fingerprint_cancel(struct fingerprint_device __unused *dev);

        int fingerprint_remove(struct fingerprint_device __unused *dev,
                                uint32_t __unused gid, uint32_t __unused fid);
        int fingerprint_set_active_group(struct fingerprint_device __unused *dev,
                                uint32_t __unused gid,
                                const char __unused *store_path);
        int fingerprint_authenticate(struct fingerprint_device __unused *dev,
                                    uint64_t __unused operation_id, __unused uint32_t gid);
        int set_notify_callback(struct fingerprint_device *dev,
                                fingerprint_notify_t notify);
        int  connect_silead_deamon();
        void enrollCredentialRSP (int index, int percent, int result, int area);
        void identifyCredentialRSP (int index, int result, int fingerid,
                                    int userId, SLFpsvcFPEnrollParams *pParams);

        int32_t setFPScreenStatus(int32_t screenStatus);
        int32_t setFPEnableCredential(int32_t index,int32_t enable);
        int32_t getFPEnableCredential(int32_t index);
        int32_t getFPVirtualKeyCode();
        int32_t setFPVirtualKeyCode(int virtualKeyCode);
        int32_t getFPLongPressVirtualKeyCode();
        int32_t setFPLongPressVirtualKeyCode(int virtualKeyCode);
        int32_t getFPDouClickVirtualKeyCode();
        int32_t setFPDouClickVirtualKeyCode(int virtualKeyCode);
        int32_t getFPVirtualKeyState();
        int32_t setFPVirtualKeyState(int virtualKeyState);
        int32_t getFPWakeUpState();
        int32_t setFPWakeUpState(int wakeUpState);
        int32_t getFingerPrintState();
        int32_t setFingerPrintState(int32_t fingerPrintState);
        int32_t setFPPowerFuncKeyState(int32_t funcKeyState);
        int32_t getFPPowerFuncKeyState();
        int32_t setFPIdleFuncKeyState(int32_t funcKeyState);
        int32_t getFPIdleFuncKeyState();
        int32_t setFPWholeFuncKeyState(int funcKeyState);
        int32_t setFPFunctionKeyState(int32_t index, int32_t enable);
        int32_t getFPFunctionKeyState(int32_t index);

        static SileadFingerprint* getInstance()
        {
            if (sInstance == NULL) {
                sInstance = new SileadFingerprint();
                sInstance->connect_silead_deamon();
            }
            return sInstance;
        }
        private:
            static SileadFingerprint *sInstance;
            AInfFpsvcClient* mClient = NULL;
            AInfFpsvcFPApkRelayerCB* mRelayer = NULL;
            fingerprint_notify_t mNotifyFunc;
            fingerprint_msg_t mMsg;
            SLFpsvcIndex_t mInfo;
            int mLastPercent;
            int mStep;
            SileadFingerprint();
            virtual ~SileadFingerprint();

};

static int silead_fingerprint_post_enroll(struct fingerprint_device __unused *dev)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->fingerprint_post_enroll(dev);
    }
    return FINGERPRINT_ERROR;
}

static uint64_t silead_fingerprint_pre_enroll(struct fingerprint_device __unused *dev)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->fingerprint_pre_enroll(dev);
    }
    return FINGERPRINT_ERROR;
}

static int silead_fingerprint_enroll(struct fingerprint_device __unused *dev,
                                const hw_auth_token_t __unused *hat,
                                uint32_t __unused gid,
                                uint32_t __unused timeout_sec)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->fingerprint_enroll(dev,hat,gid,timeout_sec);
    }
    return FINGERPRINT_ERROR;
}

static uint64_t silead_fingerprint_get_auth_id(struct fingerprint_device __unused *dev)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->fingerprint_get_auth_id(dev);
    }
    return FINGERPRINT_ERROR;
}

static int silead_fingerprint_cancel(struct fingerprint_device __unused *dev)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->fingerprint_cancel(dev);
    }
    return FINGERPRINT_ERROR;
}

static int silead_fingerprint_remove(struct fingerprint_device __unused *dev,
                                uint32_t __unused gid, uint32_t __unused fid)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->fingerprint_remove(dev,gid,fid);
    }
    return FINGERPRINT_ERROR;
}

static int silead_fingerprint_set_active_group(struct fingerprint_device __unused *dev,
            uint32_t __unused gid, const char __unused *store_path)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->fingerprint_set_active_group(dev,gid,store_path);
    }
    return FINGERPRINT_ERROR;
}

static int silead_fingerprint_authenticate(struct fingerprint_device __unused *dev,
        uint64_t __unused operation_id, __unused uint32_t gid)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->fingerprint_authenticate(dev,operation_id,gid);
    }
    return FINGERPRINT_ERROR;
}

static int silead_set_notify_callback(struct fingerprint_device *dev,
                                fingerprint_notify_t notify)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->set_notify_callback(dev,notify);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPScreenStatus(int32_t screenStatus)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPScreenStatus(screenStatus);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPEnableCredential(int32_t index, int32_t enable)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPEnableCredential(index, enable);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_getFPEnableCredential(int32_t index)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->getFPEnableCredential(index);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_getFPVirtualKeyCode()
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if(pInstance)
    {
        return pInstance->getFPVirtualKeyCode();
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPVirtualKeyCode(int virtualKeyCode)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPVirtualKeyCode(virtualKeyCode);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_getFPLongPressVirtualKeyCode()
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->getFPLongPressVirtualKeyCode();
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPLongPressVirtualKeyCode(int virtualKeyCode)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPLongPressVirtualKeyCode(virtualKeyCode);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_getFPDouClickVirtualKeyCode()
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->getFPDouClickVirtualKeyCode();
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPDouClickVirtualKeyCode(int virtualKeyCode)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPDouClickVirtualKeyCode(virtualKeyCode);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_getFPVirtualKeyState()
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->getFPVirtualKeyState();
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPVirtualKeyState(int virtualKeyState)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPVirtualKeyState(virtualKeyState);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_getFPWakeUpState()
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->getFPWakeUpState();
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPWakeUpState(int wakeUpState)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPWakeUpState(wakeUpState);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_getFingerPrintState()
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->getFingerPrintState();
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFingerPrintState(int32_t fingerPrintState)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFingerPrintState(fingerPrintState);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPPowerFuncKeyState(int32_t funcKeyState)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPPowerFuncKeyState(funcKeyState);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_getFPPowerFuncKeyState()
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->getFPPowerFuncKeyState();
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPIdleFuncKeyState(int32_t funcKeyState)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPIdleFuncKeyState(funcKeyState);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_getFPIdleFuncKeyState()
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->getFPIdleFuncKeyState();
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPWholeFuncKeyState(int funcKeyState)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPWholeFuncKeyState(funcKeyState);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_setFPFunctionKeyState(int32_t index, int32_t enable)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->setFPFunctionKeyState(index, enable);
    }
    return FINGERPRINT_ERROR;
}

static int32_t silead_getFPFunctionKeyState(int32_t index)
{
    SileadFingerprint *pInstance = SileadFingerprint::getInstance();
    if (pInstance) {
        return pInstance->getFPFunctionKeyState(index);
    }
    return FINGERPRINT_ERROR;
}

#endif
