#pragma once
#include <chrono>
#include <functional>
#include <stack>
#include <vector>
#include <tinyxml2.h>

namespace ignacionr::namecheap::response
{

    struct VisitorFan : tinyxml2::XMLVisitor
    {
        using with_element_t = std::function<bool(tinyxml2::XMLElement const &)>;
        using with_text_t = std::function<bool(tinyxml2::XMLText const &)>;

        VisitorFan()
        {
            handlers_.push({nullptr, {}});
        }

        bool VisitEnter(const tinyxml2::XMLElement &element, const tinyxml2::XMLAttribute * /*firstAttribute*/) override
        {
            auto const &top{handlers_.top().second};
            return top.element_sink_ ? top.element_sink_(element) : true;
        }
        virtual bool VisitExit(const tinyxml2::XMLElement &element)
        {
            if (&element == handlers_.top().first)
            {
                handlers_.pop();
            }
            return true;
        }
        bool Visit(const tinyxml2::XMLText &text) override
        {
            auto const &top{handlers_.top().second};
            return top.text_sink_ ? top.text_sink_(text) : true;
        }

        with_element_t &element_sink() { return handlers_.top().second.element_sink_; }
        with_text_t &text_sink() { return handlers_.top().second.text_sink_; }

        void push(tinyxml2::XMLElement const *el)
        {
            handlers_.push({el, {}});
        }

    private:
        struct Handlers
        {
            with_element_t element_sink_;
            with_text_t text_sink_;
        };
        std::stack<std::pair<const tinyxml2::XMLElement *, Handlers>> handlers_;
    };

    struct ApiResponse
    {
        void load(std::string const &xml)
        {
            tinyxml2::XMLDocument doc;
            doc.Parse(xml.data(), xml.size());
            VisitorFan fan;
            fan.element_sink() = [this, &fan](const tinyxml2::XMLElement &el)
            {
                if (0 == std::strcmp("ApiResponse", el.Name()))
                {
                    success = 0 == std::strcmp("OK", el.Attribute("Status"));
                }
                else if (0 == std::strcmp("Error", el.Name()))
                {
                    fan.text_sink() = [this, &fan](const tinyxml2::XMLText &text)
                    {
                        errors.push_back(text.Value());
                        fan.text_sink() = {};
                        return true;
                    };
                }
                else if (0 == std::strcmp("CommandResponse", el.Name()))
                {
                    fan.push(&el);
                    load_from(fan);
                }
                return true;
            };
            doc.Accept(&fan);
        }
        bool success;
        std::vector<std::string> errors;

    protected:
        virtual void load_from(VisitorFan &fan) = 0;
    };

    std::chrono::system_clock::time_point USDateToTimePoint(const std::string &date)
    {
        std::tm tm = {};
        std::istringstream ss(date);
        char delimiter;

        // Parse string in MM/DD/YYYY format
        ss >> tm.tm_mon >> delimiter >> tm.tm_mday >> delimiter >> tm.tm_year;

        // Adjust for tm structure (months are [0, 11], years since 1900)
        tm.tm_mon -= 1;
        tm.tm_year -= 1900;

        // Convert to time_t
        std::time_t time = std::mktime(&tm);

        // Convert to std::chrono::system_clock::time_point
        return std::chrono::system_clock::from_time_t(time);
    }
}
