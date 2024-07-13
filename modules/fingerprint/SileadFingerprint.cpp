#include <hardware/SileadFingerprint.h>

SileadFingerprint* SileadFingerprint::sInstance = NULL;

SileadFingerprint::SileadFingerprint():mNotifyFunc(NULL),mLastPercent(0),mStep(0)
{
}

SileadFingerprint::~SileadFingerprint()
{
}

void SileadFingerprint::identifyCredentialRSP(int index, int result, int fingerid,int userId,SLFpsvcFPEnrollParams *pParams)
{
    ALOGD("SileadFingerprint index=%d, result=%d fingerid=%d userId=%d\n",index,result,fingerid,userId);
    if(pParams) {
        ALOGD("pParams not null");
    } else {
        ALOGD("pParams is null");
    }
    if (mNotifyFunc == NULL) {
        ALOGE("Invalid mNotifyFunc ptr");
        return;
    }
    switch(result)
    {
        case SL_IDENTIFY_WAKEUP_MATCHED:
        case SL_SUCCESS:
        {
            ALOGD("notify the onAcquired begin");
            mMsg.type = FINGERPRINT_ACQUIRED;
            mMsg.data.acquired.acquired_info = FINGERPRINT_ACQUIRED_GOOD;
            mNotifyFunc(&mMsg);
            ALOGD("notify the acquired end");

            mMsg.type = FINGERPRINT_AUTHENTICATED;
            mMsg.data.authenticated.finger.fid = fingerid+1;
            mMsg.data.authenticated.finger.gid = userId;
            if(pParams) {
                memcpy(&mMsg.data.authenticated.hat,pParams->token,sizeof(hw_auth_token_t));
            }
            mNotifyFunc(&mMsg);
            ALOGD("notify the authenticated end");
            break;
        }
        case SL_IDENTIFY_WAKEUP_NOT_MATCHED:
        case SL_IDENTIFY_ERR_MATCH:
        {
            ALOGD("notify the onAcquired begin");
            mMsg.type = FINGERPRINT_ACQUIRED;
            mMsg.data.acquired.acquired_info = FINGERPRINT_ACQUIRED_GOOD;
            mNotifyFunc(&mMsg);
            ALOGD("notify the acquired end");

            mMsg.type = FINGERPRINT_AUTHENTICATED;
            mMsg.data.authenticated.finger.fid = 0;
            mMsg.data.authenticated.finger.gid = userId;
            mNotifyFunc(&mMsg);
            ALOGD("notify the authenticated end");
            break;
        }
        case SL_IDENTIFY_CANCELED:
        case SL_IDENTIFY_ERROR:
        case SL_IDENTIFY_FAIL:
        {
            int errInfo = FINGERPRINT_ERROR_CANCELED;
            if(result == SL_IDENTIFY_CANCELED) {
                errInfo = FINGERPRINT_ERROR_CANCELED;
            }
            mMsg.type = FINGERPRINT_ERROR;
            mMsg.data.error =(fingerprint_error_t) errInfo;
            mNotifyFunc(&mMsg);
            break;
        }
        case SL_IDENTIFY_TMEOUT:
        case SL_IDENTIFY_CURR_IMG_BAD:
        case SL_TOUCH_TOO_FAST:
        {
            int acqiredInfo = SL_IDENTIFY_CURR_IMG_BAD;
            if(result == SL_IDENTIFY_TMEOUT) {
                acqiredInfo = FINGERPRINT_ACQUIRED_TOO_SLOW;
            } else if(result == SL_IDENTIFY_CURR_IMG_BAD) {
                acqiredInfo = FINGERPRINT_ACQUIRED_IMAGER_DIRTY;
            } else if(result == SL_TOUCH_TOO_FAST) {
                acqiredInfo = FINGERPRINT_ACQUIRED_TOO_FAST;
            }
            mMsg.type = FINGERPRINT_ACQUIRED;
            mMsg.data.acquired.acquired_info = (fingerprint_acquired_info_t)acqiredInfo;
            mNotifyFunc(&mMsg);
            break;
        }
        default:
        {
            break;
        }
    }
}

void SileadFingerprint::enrollCredentialRSP(int index, int percent, int result, int area)
{
    ALOGD("enrollCredentialRSP index=%d, mLastPercent=%d, percent=%d, result=%d,area=%d", index, mLastPercent, percent, result, area);
    if (mNotifyFunc == NULL) {
        ALOGE("Invalid mNotifyFunc ptr");
        return;
    }
    mClient->GetFPInfo(&mInfo);
    switch(result){
        case SL_SUCCESS:
        {
            //Enroll success
            /*if(mLastPercent >= percent) {
                mLastPercent = 0;
            }
            int step = percent - mLastPercent;
            mLastPercent = percent;
            float remaing = (100.0f- percent)/step;
            int intRemaing = (int)remaing;
            if(remaing-intRemaing>0.5) {
                intRemaing++;
            }
            ALOGD("enrollCredentialRSP success intRemaing=%d,remaing=%f,step=%d",intRemaing,remaing,step);*/
            if(mLastPercent >= percent) {
                mLastPercent = 0;
            }
            if (mLastPercent == 0) {
                mStep = 100/percent;
            }
            if (percent > mLastPercent) {
                if (mStep > 0) {
                    mStep--;
                }
            }
            mLastPercent = percent;
            int intRemaing = mStep;
            ALOGD("enrollCredentialRSP success intRemaing=%d",intRemaing);
            mMsg.type = FINGERPRINT_TEMPLATE_ENROLLING;
            mMsg.data.enroll.finger.fid = index+1;
            mMsg.data.enroll.finger.gid = mInfo.userid;
            mMsg.data.enroll.samples_remaining = intRemaing;
            mNotifyFunc(&mMsg);
            break;
        }
        case SL_NOT_SUPPORT:
        case SL_ENROLL_ERROR:
        case SL_ENROLL_FAIL:
        case SL_ENROLL_CURR_ENR_FAIL:
        {
            int errInfo = FINGERPRINT_ERROR_CANCELED;
            mMsg.type = FINGERPRINT_ERROR;
            mMsg.data.error =(fingerprint_error_t) errInfo;
            mNotifyFunc(&mMsg);
            break;
        }
        case SL_ENROLL_CURR_IMG_BAD:
        case SL_ENROLL_CHECK_ERROR:
        case SL_ENROLL_ERROR_LOW_COVERAGE:
        case SL_ENROLL_ERROR_LOW_QUALITY:
        case SL_ENROLL_ERROR_SAME_AREA:
        {
            int  acqiredInfo = SL_ENROLL_ERROR_LOW_QUALITY;
            if(result == SL_ENROLL_CURR_IMG_BAD || result == SL_ENROLL_ERROR_LOW_QUALITY) {
                acqiredInfo = FINGERPRINT_ACQUIRED_IMAGER_DIRTY;
            } else if(result == SL_ENROLL_ERROR_LOW_COVERAGE) {
                acqiredInfo = FINGERPRINT_ACQUIRED_PARTIAL;
            } else if(result == SL_ENROLL_ERROR_LOW_COVERAGE) {
                acqiredInfo = FINGERPRINT_ACQUIRED_INSUFFICIENT;
            }
            mMsg.type = FINGERPRINT_ACQUIRED;
            mMsg.data.acquired.acquired_info = (fingerprint_acquired_info_t)acqiredInfo;
            mNotifyFunc(&mMsg);
            break;
        }
    }
}

uint64_t SileadFingerprint::fingerprint_pre_enroll(struct fingerprint_device __unused *dev)
{
    ALOGD("SileadFingerprintDaemonProxy preEnroll");
    if(!mClient) {
        ALOGD("preEnroll No client");
    }
    return mClient->PreEnroll();
}

int SileadFingerprint::fingerprint_post_enroll(struct fingerprint_device __unused *dev)
{
    ALOGD("SileadFingerprintDaemonProxy PostEnroll");
    if(!mClient) {
        ALOGD("PostEnroll No client");
    }
    return mClient->PostEnroll();
}

int SileadFingerprint::fingerprint_enroll(struct fingerprint_device __unused *dev,
        const hw_auth_token_t __unused *hat,
        uint32_t __unused gid,
        uint32_t __unused timeout_sec)
{
    ALOGD("enroll(gid=%d, timeout=%d),token=%p\n", gid, timeout_sec,hat);
    int32_t ret = 0;
    if(!mClient) {
        ALOGD("enroll No client");
        return -1;
    }
    int32_t newGroupId = gid;
    mClient->GetFPInfo(&mInfo);
    ALOGD("mInfo.userid=%d,gid=%d",mInfo.userid,gid);
    SLFpsvcFPEnrollParams enrollParams;
    memset(&enrollParams,0,sizeof(SLFpsvcFPEnrollParams));
    enrollParams.tokenSize = sizeof(hw_auth_token_t);
    memcpy(enrollParams.token,hat,sizeof(hw_auth_token_t));
    int enrollIndex = -1;
    if(mInfo.userid!=newGroupId) {  
        fingerprint_set_active_group(NULL,newGroupId,NULL);
    }

    //get enroll index
    for(int i=0;i<SL_SLOT_ALL;i++) {
        if(mInfo.fpinfo[i].slot == 0) {
            enrollIndex = i;
            break;
        }
    }
    mLastPercent = 0;
    if(enrollIndex!=-1) {
        ret = mClient->EnrollCredential(enrollIndex,&enrollParams,timeout_sec * 1000);
    } else {
        ALOGD("SileadFingerprintDaemonProxy can not enroll more fingerprint");
        ret = -1;
    }
    return ret;
}

uint64_t SileadFingerprint::fingerprint_get_auth_id(struct fingerprint_device __unused *dev)
{
    ALOGD("enter fingerprint_get_auth_id");
    if (mClient == NULL) {
        ALOGE("getAuthenticatorId NO client \n");
        return -1;
    }
    return mClient->GetAuthenticatorId();
}

int SileadFingerprint::fingerprint_cancel(struct fingerprint_device __unused *dev)
{
    ALOGD( "enter fingerprint_cancel()\n");
    if(!mClient) {
        ALOGD("fingerprint_cancel No client");
    }
    int ret = mClient->FpCancelOperation();
    return ret;
}

int32_t SileadFingerprint::setFPScreenStatus(int32_t screenStatus)
{
    ALOGD("setFPScreenStatus screenStatus=%d \n", screenStatus);
    if (mClient == NULL) {
        ALOGE("setFPScreenStatus NO client \n");
        return -1;
    }
    return mClient->SetFPScreenStatus(screenStatus);
}

int32_t SileadFingerprint::setFPEnableCredential(int32_t index, int32_t enable)
{
     ALOGD("setFPEnableCredential index=%d ,enable=%d \n", index, enable);
     if (mClient == NULL) {
         ALOGE("setFPEnableCredential NO client \n");
         return -1;
     }
     mClient->GetFPInfo(&mInfo);
     mInfo.fpinfo[index-1].enable = enable;
     return  mClient->SetFPInfo(&mInfo);
}

int32_t SileadFingerprint::getFPEnableCredential(int32_t index)
{
    ALOGD("getFPEnableCredential index=%d  \n", index);
    if (mClient == NULL) {
        ALOGE("getFPEnableCredential NO client \n");
        return -1;
    }
    mClient->GetFPInfo(&mInfo);
    return mInfo.fpinfo[index-1].enable;
}

int32_t SileadFingerprint::setFPFunctionKeyState(int32_t index, int32_t enable)
{
     ALOGD("setFPFunctionKeyState index=%d ,enable=%d \n", index, enable);
     if (mClient == NULL) {
         ALOGE("setFPFunctionKeyState NO client \n");
         return -1;
     }
     mClient->GetFPInfo(&mInfo);
     mInfo.fpinfo[index-1].functionkeyon = enable;
     return  mClient->SetFPInfo(&mInfo);
}

int32_t SileadFingerprint::getFPFunctionKeyState(int32_t index)
{
    ALOGD("getFPFunctionKeyState index=%d  \n", index);
    if (mClient == NULL) {
        ALOGE("getFPFunctionKeyState NO client \n");
        return -1;
    }
    mClient->GetFPInfo(&mInfo);
    return mInfo.fpinfo[index-1].functionkeyon;
}

int32_t SileadFingerprint::getFPVirtualKeyCode()
{
     ALOGD("getFPVirtualKeyCode");
     if (mClient == NULL) {
         ALOGE("getFPVirtualKeyCode NO client \n");
         return -1;
     }
     GENERAL_PARAM param = SL_GET_VIRTUAL_KEY_CODE;
     return  mClient->GeneralCall(param, 0);
}

int32_t SileadFingerprint::setFPVirtualKeyCode(int virtualKeyCode)
{
    ALOGD("setFPVirtualKeyCode index=%d  \n", virtualKeyCode);
    if (mClient == NULL) {
        ALOGE("setFPVirtualKeyCode NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_SET_VIRTUAL_KEY_CODE;
    return mClient->GeneralCall(param, virtualKeyCode);
}

int32_t SileadFingerprint::getFPLongPressVirtualKeyCode()
{
    ALOGD("setFPVirtualKeyCode\n");
    if (mClient == NULL) {
         ALOGE("getFPLongPressVirtualKeyCode NO client \n");
         return -1;
    }
    GENERAL_PARAM param = SL_GET_VIRTUAL_KEY_CODE_LONG_PRESS;
    return mClient->GeneralCall(param, 0);
}

int32_t SileadFingerprint::setFPLongPressVirtualKeyCode(int virtualKeyCode)
{
    ALOGD("setFPLongPressVirtualKeyCode VirtualKeyCode=%d  \n", virtualKeyCode);
    if (mClient == NULL) {
        ALOGE("setFPLongPressVirtualKeyCode NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_SET_VIRTUAL_KEY_CODE_LONG_PRESS;
    return mClient->GeneralCall(param, virtualKeyCode);
}

int32_t SileadFingerprint::getFPDouClickVirtualKeyCode()
{
    ALOGD("getFPDouClickVirtualKeyCode \n");
    if (mClient == NULL) {
        ALOGE("getFPDouClickVirtualKeyCode NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_GET_VIRTUAL_KEY_CODE_DOUBULE_CLICK;
    return mClient->GeneralCall(param, 0);
}

int32_t SileadFingerprint::setFPDouClickVirtualKeyCode(int virtualKeyCode)
{
    ALOGD("setFPDouClickVirtualKeyCode VirtualKeyCode =%d \n",virtualKeyCode);
    if (mClient == NULL) {
        ALOGE("setFPDouClickVirtualKeyCode NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_SET_VIRTUAL_KEY_CODE_DOUBULE_CLICK;
    return mClient->GeneralCall(param, virtualKeyCode);
}

int32_t SileadFingerprint::getFPVirtualKeyState()
{
    ALOGD("getFPVirtualKeyState  \n");
    if (mClient == NULL) {
        ALOGE("getFPVirtualKeyState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_GET_VIRTUAL_KEY_STATE;
    return mClient->GeneralCall(param, 0);
}

int32_t SileadFingerprint::setFPVirtualKeyState(int virtualKeyState)
{
    ALOGD("setFPVirtualKeyState  VirtualKeyState=%d \n",virtualKeyState);
    if (mClient == NULL) {
        ALOGE("setFPVirtualKeyState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_SET_VIRTUAL_KEY_STATE;
    return mClient->GeneralCall(param, virtualKeyState);
}

int32_t SileadFingerprint::getFPWakeUpState()
{
    ALOGD("getFPWakeUpState \n");
    if (mClient == NULL) {
        ALOGE("getFPWakeUpState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_GET_WAKE_UP_STATE;
    return mClient->GeneralCall(param, 0);
}

int32_t SileadFingerprint::setFPWakeUpState(int wakeUpState)
{
    ALOGD("setFPWakeUpState WakeUpState=%d \n",wakeUpState);
    if (mClient == NULL) {
        ALOGE("setFPWakeUpState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_SET_WAKE_UP_STATE;
    return mClient->GeneralCall(param, wakeUpState);
}

int32_t SileadFingerprint::getFingerPrintState()
{
    ALOGD("getFingerPrintState \n");
    if (mClient == NULL) {
        ALOGE("getFingerPrintState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_GET_FINGER_PRINT_STATE;
    return mClient->GeneralCall(param, 0);
}

int32_t SileadFingerprint::setFingerPrintState(int32_t fingerPrintState)
{
    ALOGD("setFingerPrintState FingerPrintState=%d\n",fingerPrintState);
    if (mClient == NULL) {
        ALOGE("setFingerPrintState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_SET_FINGER_PRINT_STATE;
    return mClient->GeneralCall(param, fingerPrintState);
}

int32_t SileadFingerprint::setFPPowerFuncKeyState(int32_t funcKeyState)
{
    ALOGD("setFPPowerFuncKeyState FuncKeyState=%d\n",funcKeyState);
    if (mClient == NULL) {
        ALOGE("setFPPowerFuncKeyState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_SET_POWER_FUNC_KEY_STATE;
    return mClient->GeneralCall(param, funcKeyState);
}

int32_t SileadFingerprint::getFPPowerFuncKeyState()
{
    ALOGD("getFPPowerFuncKeyState \n");
    if (mClient == NULL) {
        ALOGE("getFPPowerFuncKeyState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_GET_POWER_FUNC_KEY_STATE;
    return mClient->GeneralCall(param, 0);
}

int32_t    SileadFingerprint::setFPIdleFuncKeyState(int32_t funcKeyState)
{
    ALOGD("setFPIdleFuncKeyState  FuncKeyState =%d \n",funcKeyState);
    if (mClient == NULL) {
        ALOGE("setFPIdleFuncKeyState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_SET_IDLE_FUNC_KEY_STATE;
    return mClient->GeneralCall(param, funcKeyState);
}

int32_t SileadFingerprint::getFPIdleFuncKeyState()
{
    ALOGD("getFPIdleFuncKeyState  \n");
    if (mClient == NULL) {
        ALOGE("getFPIdleFuncKeyState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_GET_IDLE_FUNC_KEY_STATE;
    return mClient->GeneralCall(param, 0);
}

int32_t SileadFingerprint::setFPWholeFuncKeyState(int funcKeyState)
{
    ALOGD("setFPWholeFuncKeyState  FuncKeyState =%d \n",funcKeyState);
    if (mClient == NULL) {
        ALOGE("setFPWholeFuncKeyState NO client \n");
        return -1;
    }
    GENERAL_PARAM param = SL_SET_WHOLE_FUNC_KEY_STATE;
    return mClient->GeneralCall(param, funcKeyState);
}

int SileadFingerprint::fingerprint_remove(struct fingerprint_device __unused *dev,
        uint32_t __unused gid, uint32_t __unused fid)
{
    ALOGD("fingerprint_remove(fid=%d, gid=%d)\n", fid, gid);
    if (mClient == NULL) {
         ALOGE("remove NO client \n");
         return -1;
    }
    if(fid <=0) {
        ALOGE("fingerId invalide,return");
    }
    int ret;
    mClient->GetFPInfo(&mInfo);
    ALOGD("mInfo.userid=%d,groupId=%d",mInfo.userid,gid);
    if(mInfo.userid!=gid){
        fingerprint_set_active_group(NULL,gid,NULL);
        ret = mClient->RemoveCredential(fid - 1);
        fingerprint_set_active_group(NULL,mInfo.userid,NULL);
    } else {
        ret = mClient->RemoveCredential(fid-1);
    }
    if (mNotifyFunc != NULL) {
        ALOGD("callback not null");
        if(ret == 0) {
            mMsg.type = FINGERPRINT_TEMPLATE_REMOVED;
            mMsg.data.removed.finger.fid = fid;
            mMsg.data.removed.finger.gid = gid;
            mNotifyFunc(&mMsg);
        }
    } else {
        ALOGE("error callback is null");
    }
    return ret;
}

 int SileadFingerprint::fingerprint_set_active_group(struct fingerprint_device __unused *dev,
        uint32_t __unused gid, const char __unused *store_path)
{
    ALOGD("setActiveGroup gid=%d,store_path=%s",gid,store_path);
    int ret = -1;
    if(!mClient) {
        ret = mClient->SwitchUser(gid);
    }
    return ret;
}

 int SileadFingerprint::fingerprint_authenticate(struct fingerprint_device __unused *dev,
        uint64_t __unused operation_id, __unused uint32_t gid)
{
    //ALOGD("authenticate(operation_id=%d PRId64 ", gid=%d)\n", operation_id, gid);
    if (mClient == NULL) {
        ALOGE("fingerprint_authenticate NO client \n");
        return -1;
    }
    int ret;
    mClient->GetFPInfo(&mInfo);
    ALOGD("mInfo.userid=%d,groupId=%d",mInfo.userid,gid);
    if(mInfo.userid!=gid) {
        fingerprint_set_active_group(NULL,gid,NULL);
        ret = mClient->IdentifyCredential(0,operation_id);
    } else {
        ret = mClient->IdentifyCredential(0,operation_id);
    }
    return ret;
}

int SileadFingerprint::set_notify_callback(struct fingerprint_device *dev,
        fingerprint_notify_t notify)
{
    if(!notify) {
        return -1;
    }
    mNotifyFunc = notify;
    return 0;
}

int SileadFingerprint::connect_silead_deamon()
{
    if (!mRelayer) {
        mRelayer = reinterpret_cast<AInfFpsvcFPApkRelayerCB*>(AInfFpsvcFPApkRelayerCB::Create());
    }

    if (!mClient) {
        mClient = AInfFpsvcClient::Create(mRelayer);
    }
    if(mClient) {
        mClient->SetToMMode();
    }
    return 0;
}


