#pragma once
#include <string>

namespace ignacionr::namecheap
{

    struct ContactInfo
    {
        std::string FirstName;
        std::string LastName;
        std::string Address1;
        std::string City;
        std::string StateProvince;
        std::string PostalCode;
        std::string Country;
        std::string Phone;
        std::string EmailAddress;
        template<typename strategy_t>
        void save_to(strategy_t &strategy) const
        {
            strategy("FirstName", FirstName);
            strategy("LastName", LastName);
            strategy("Address1", Address1);
            strategy("City", City);
            strategy("StateProvince", StateProvince);
            strategy("PostalCode", PostalCode);
            strategy("Country", Country);
            strategy("Phone", Phone);
            strategy("EmailAddress", EmailAddress);
        }
    };

    struct RegistrationParams
    {
        ContactInfo Registrant;
        ContactInfo Tech;
        ContactInfo Admin;
        ContactInfo AuxBilling;

        void CopyRegistrantToAllContacts()
        {
            Tech = Registrant;
            Admin = Registrant;
            AuxBilling = Registrant;
        }

        template<typename strategy_t>
        void save_to(strategy_t &strategy) const
        {
            auto registrant_strategy{[&strategy](std::string name, auto const &value)
                                     { strategy("Registrant" + name, value); }};
            Registrant.save_to(registrant_strategy);
            auto tech_strategy{[&strategy](std::string name, auto const &value)
                               { strategy("Tech" + name, value); }};
            Tech.save_to(tech_strategy);
            auto admin_strategy{[&strategy](std::string name, auto const &value)
                                { strategy("Admin" + name, value); }};
            Admin.save_to(admin_strategy);
            auto auxbilling_strategy{[&strategy](std::string name, auto const &value)
                                     { strategy("AuxBilling" + name, value); }};
            AuxBilling.save_to(auxbilling_strategy);
        }
    };
}