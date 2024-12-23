#pragma once
#include <optional>
#include <map>
#include <vector>
template <typename Index, typename Element>
class genericContext : private std::vector<std::map<Index, Element>>
{
    
    public:
    // std::vector<std::map<Index, Element>> contextStack;
    void enterScope()
    {
        std::vector<std::map<Index, Element>>::push_back(std::map<Index, Element>());
    }
    void exitScope()
    {
        // /*contextStack.*/pop_back();
        std::vector<std::map<Index, Element>>::pop_back();
    }
    std::optional<Element> get(Index index)
    {
        // for (auto it = /*contextStack*/.rbegin(); it != /*contextStack*/.rend(); it++)
        for (auto it = std::vector<std::map<Index, Element>>::rbegin(); it != std::vector<std::map<Index, Element>>::rend(); it++)
        {
            if (it->find(index) != it->end())
            {
                return it->at(index);
            }
        }
        return std::nullopt;
    }
    bool has(Index index)
    {
        return get(index).has_value();
    }
    
    void add(Index index, Element element)
    {
        std::vector<std::map<Index, Element>>::back()[index] = element;
    }

    void remove(Index index)
    {
        for (auto it = std::vector<std::map<Index, Element>>::rbegin(); it != std::vector<std::map<Index, Element>>::rend(); it++)
        {
            if (it->find(index) != it->end())
            {
                it->erase(index);
                return;
            }
        }
    }

    genericContext()
    {
        std::vector<std::map<Index, Element>>::push_back(std::map<Index, Element>());
    }
    
    std::map<Index, Element> &getCurrentContext()
    {
        return std::vector<std::map<Index, Element>>::back();
    }
};