﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DBServer
{
    public enum ERROR_CODE
    {
        NONE = 0,

        REDIS_START_SET_TEST = 21,
        REDIS_START_EXCEPTION = 22,


        PREV_REQUEST_NOT_COMPLETE = 101,
        PREV_REQUEST_FAIL_REDIS = 102,

        REQ_CREATE_USER_INVALID_ID = 111,
        REQ_CREATE_USER_DUPLICATE_USER_ID = 112,

        REQ_LOGIN_INVALID_USER = 121,
        REQ_LOGIN_INVALID_PW = 122,
        REQ_LOGIN_NO_DATA = 123,

        REQ_LOGOUT_FAIL_CHECK_ID_OR_AUTHTOKEN = 123,

        REQ_LOAD_BASIC_GAME_DATA_INVALID_AUTH = 131,
        REQ_LOAD_BASIC_GAME_DATA_INVALID_ID = 132,
    }
}
