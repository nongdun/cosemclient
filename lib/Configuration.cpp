/**
 * Configuration file for Cosem client engine
 *
 * Copyright (c) 2016, Anthony Rabine
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the BSD license.
 * See LICENSE.txt for more details.
 *
 */

#include <iostream>
#include <cstdint>
#include "Configuration.h"


Configuration::Configuration()
	: timeout_connect(3U)
    , timeout_dial(70U)
    , timeout_request(5U)
	, retries(0)
{

}

/*
{
    "version": "1.0.0",

    "session": {
        "phy_layer": "serial",

        "modem": {
            "enable": true,
            "init": "AT",
            "phone": "0631500899"
        },

        "retries": 1,

        "timeouts": {
            "dial": 90,
            "connect": 5,
            "request": 5
        }
    },

    "meters": [
        {
            "id": "saphir0899",
            "transport": "hdlc",
            "hdlc": {
                "phy_addr": 17,
                "address_size": 4,
                "test_addr": false
            },
            "cosem": {
                "auth_level": "LOW_LEVEL_SECURITY",
                "auth_value": "ABCDEFGH",
                "client": 1,
                "logical_device": 1
            }
        }
    ]
}

*/

// Very tolerant, use default values of classes if corresponding parameter is not found
void Configuration::ParseSessionFile(const std::string &file)
{
    JsonReader reader;
    JsonValue json;
    JsonValue val;

    if (reader.ParseFile(json, file))
    {
        JsonValue session = json.FindValue("session");
        if (session.IsObject())
        {
            val= session.FindValue("retries");
            if (val.IsInteger())
            {
                retries = static_cast<uint32_t>(val.GetInteger());
            }

            // *********************************   MODEM   *********************************

            JsonValue modemObj = json.FindValue("modem");
            if (modemObj.IsObject())
            {
                val = modemObj.FindValue("phone");
                if (val.IsString())
                {
                    modem.phone = val.GetString();
                }

                val = modemObj.FindValue("enable");
                if (val.IsBoolean())
                {
                    modem.useModem = val.GetBool();
                }

                val = modemObj.FindValue("init");
                if (val.IsString())
                {
                    modem.init = val.GetString();
                }
            }

            // *********************************   TIMEOUTS   *********************************
            JsonValue timeoutsObj = session.FindValue("timeouts");
            if (timeoutsObj.IsObject())
            {
                val = timeoutsObj.FindValue("dial");
                if (val.IsInteger())
                {
                    timeout_dial = static_cast<uint32_t>(val.GetInteger());
                }

                val = timeoutsObj.FindValue("connect");
                if (val.IsInteger())
                {
                    timeout_connect = static_cast<uint32_t>(val.GetInteger());
                }

                val = timeoutsObj.FindValue("request");
                if (val.IsInteger())
                {
                    timeout_request = static_cast<uint32_t>(val.GetInteger());
                }
            }
        }

        JsonValue meterObj = json.FindValue("meters");
        if (meterObj.IsArray())
        {

            for (JsonArray::Iterator iter = meterObj.GetArray().Begin(); iter != meterObj.GetArray().End(); ++iter)
            {
                Meter meter;
                if (iter->IsObject())
                {
                    val = iter->FindValue("id");
                    if (val.IsString())
                    {
                        meter.meterId = val.GetString();
                    }

                    // *********************************   COSEM   *********************************
                    JsonValue cosemObj = iter->FindValue("cosem");
                    if (cosemObj.IsObject())
                    {
                        val = cosemObj.FindValue("auth_value");
                        if (val.IsString())
                        {
                            meter.cosem.auth_value = val.GetString();
                        }

                        val = cosemObj.FindValue("auth_level");
                        if (val.IsString())
                        {
                            meter.cosem.auth_level = val.GetString();
                        }


                        val = cosemObj.FindValue("client");
                        if (val.IsInteger())
                        {
                            meter.hdlc.client_addr = static_cast<unsigned int>(val.GetInteger());
                        }

                        val = cosemObj.FindValue("logical_device");
                        if (val.IsInteger())
                        {
                            meter.hdlc.logical_device = static_cast<unsigned int>(val.GetInteger());
                        }
                    }

                    // *********************************   HDLC   *********************************
                    JsonValue hdlcObj = iter->FindValue("hdlc");
                    if (hdlcObj.IsObject())
                    {
                        val = hdlcObj.FindValue("phy_addr");
                        if (val.IsInteger())
                        {
                            meter.hdlc.phy_address = static_cast<unsigned int>(val.GetInteger());
                        }

                        val = hdlcObj.FindValue("address_size");
                        if (val.IsInteger())
                        {
                            meter.hdlc.addr_len = static_cast<unsigned int>(val.GetInteger());
                        }

                        val = meterObj.FindValue("test_addr");
                        if (val.IsBoolean())
                        {
                            meter.testHdlcAddr = val.GetBool();
                        }
                    }
                }
            }
        }

    }
    else
    {
        std::cout << "** Error opening file: " << file << std::endl;
    }
}


// Very tolerant, use default values of classes if corresponding parameter is not found
void Configuration::ParseComFile(const std::string &file, Transport::Params &comm)
{
    JsonReader reader;
    JsonValue json;

    if (reader.ParseFile(json, file))
    {
        JsonValue portObj = json.FindValue("serial");
        if (portObj.IsObject())
        {
            JsonValue val = portObj.FindValue("port");
            if (val.IsString())
            {
                comm.port = val.GetString();
                val = portObj.FindValue("baudrate");
                if (val.IsInteger())
                {
                    comm.baudrate = static_cast<unsigned int>(val.GetInteger());
                }
            }
        }
    }
    else
    {
        std::cout << "** Error opening file: " << file << std::endl;
    }

}

void Configuration::ParseObjectsFile(const std::string &file)
{
    JsonReader reader;
    JsonValue json;

    if (reader.ParseFile(json, file))
    {
        JsonValue val = json.FindValue("objects");
        if (val.IsArray())
        {
            JsonArray arr = val.GetArray();
            for (std::uint32_t i = 0U; i < arr.Size(); i++)
            {
                Object object;
                JsonValue obj = arr.GetEntry(i);

                val = obj.FindValue("name");
                if (val.IsString())
                {
                    object.name = val.GetString();
                }
                val = obj.FindValue("logical_name");
                if (val.IsString())
                {
                    object.ln = val.GetString();
                }
                val = obj.FindValue("class_id");
                if (val.IsInteger())
                {
                    object.class_id = static_cast<std::uint16_t>(val.GetInteger());
                }
                val = obj.FindValue("attribute_id");
                if (val.IsInteger())
                {
                    object.attribute_id = static_cast<std::int8_t>(val.GetInteger());
                }

                object.Print();
                list.push_back(object);
            }
        }
    }
    else
    {
        std::cout << "** Error opening file: " << file << std::endl;
    }
}

