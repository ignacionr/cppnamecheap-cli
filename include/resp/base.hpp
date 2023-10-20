#pragma once
#include <chrono>
#include <exception>
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

        auto &push(tinyxml2::XMLElement const *el)
        {
            handlers_.push({el, {}});
            return *this;
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
        
        std::string save()
        {
            tinyxml2::XMLDocument doc;
            
            // Create the root ApiResponse element
            tinyxml2::XMLElement* root = doc.NewElement("ApiResponse");
            root->SetAttribute("Status", success ? "OK" : "Failed");
            doc.InsertEndChild(root);

            // Add Errors, if any
            for(const auto& err : errors)
            {
                tinyxml2::XMLElement* errElem = doc.NewElement("Error");
                tinyxml2::XMLText* errText = doc.NewText(err.c_str());
                errElem->InsertEndChild(errText);
                root->InsertEndChild(errElem);
            }

            // Call the derived class specific serialization method
            save_to(doc, root);

            // Serialize to string
            tinyxml2::XMLPrinter printer;
            doc.Print(&printer);
            return printer.CStr();
        }

        bool success;
        std::vector<std::string> errors;

    protected:
        virtual void load_from(VisitorFan &fan) = 0;
        virtual void save_to(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root) const {
            throw std::runtime_error("This class does not support saving.");
        }
    };

}
