#pragma once
#include <string>
#include <optional>
#include <tinyxml2.h>

namespace ignacionr::namecheap {
    struct Host{
        Host() = default;
        
        Host(tinyxml2::XMLElement const &el) 
        {
            HostId = el.UnsignedAttribute("HostId");
            Name = el.Attribute("Name");
            Type = el.Attribute("Type");
            Address = el.Attribute("Address");
            uint vMXPref;
            if (tinyxml2::XMLError::XML_SUCCESS == el.QueryAttribute("MXPref", &vMXPref)) {
                MXPref = vMXPref;
            }
            TTL = el.UnsignedAttribute("TTL");
        }

        uint HostId;
        std::string Name;
        std::string Type;
        std::string Address;
        std::optional<uint> MXPref;
        uint TTL;
    };
}
