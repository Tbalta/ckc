template <typename Index, typename Element>
class genericContext
{
    public:
    std::vector<std::map<Index, Element>> contextStack;
    void enterScope()
    {
        contextStack.push_back(std::map<Index, Element>());
    }
    void exitScope()
    {
        contextStack.pop_back();
    }
    std::optional<Element> get(Index index)
    {
        for (auto it = contextStack.rbegin(); it != contextStack.rend(); it++)
        {
            if (it->find(index) != it->end())
            {
                return it->at(index);
            }
        }
        return std::nullopt;
    }
    void add(Index index, Element element)
    {
        contextStack.back()[index] = element;
    }
    genericContext()
    {
        contextStack.push_back(std::map<Index, Element>());
    }
};