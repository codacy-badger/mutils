#include "mutils/algorithm/containerutils.h"
#include "mutils/string/stringutils.h"
#include "mutils/logger/logger.h"
#include "mutils/compile/meta_tags.h"

#include <functional>
#include <vector>
#include <string>

namespace MUtils
{

MetaTags_t parse_meta_tags(const std::string& text)
{
    MetaTags_t tags;
    try
    {
        std::vector<std::string> lines;
        string_split_lines(text, lines);

        int32_t lineNumber = 0;
        for (auto& line : lines)
        {
            auto pos = line.find("//", 0);
            if (pos != std::string::npos)
            {
                std::vector<std::string> tokens;
                string_split(line.substr(pos + 2), " \t,", tokens);

                // Always lower case
                for (int tok = 0; tok < tokens.size(); tok++)
                {
                    tokens[tok] = string_tolower(tokens[tok]);
                }

                if (tokens.size() % 2 == 0)
                {
                    auto keyValuePairs = vector_convert_to_pairs(tokens);
                    for (auto& keyValue : keyValuePairs)
                    {
                        tags[keyValue.first] = TagValue { keyValue.second, lineNumber };
                    }
                }
            }
            lineNumber++;
        }
    }
    catch (std::exception & ex)
    {
        LOG(DEBUG) << ex.what();
    }
    return tags;
}

}; // namespace Jorvik
