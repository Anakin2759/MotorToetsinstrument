#pragma once
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <iostream>
#include <regex>
#include <string>
#include <type_traits>
#include <unordered_map>

class JsonParser
{
public:
    JsonParser()  = default;
    ~JsonParser() = default;

    std::unordered_map<std::string, std::string> operator()(const char *jsonString)
    {
        return parseJson(jsonString);
    }

    std::unordered_map<std::string, std::string> operator()(const std::string &jsonString)
    {
        return parseJson(jsonString.c_str());
    }

private:
    std::unordered_map<std::string, std::string> parseJson(const char *jsonString)
    {
        std::unordered_map<std::string, std::string> result;

        rapidjson::Document document;
        if (document.Parse(jsonString).HasParseError())
        {
            std::cerr << "JSON parse error: " << rapidjson::GetParseError_En(document.GetParseError()) << std::endl;
            return result;
        }

        // 遍历 JSON 对象
        if (document.IsObject())
        {
            for (const auto &[key, value] : document.GetObject())
            {
                if (std::string(key.GetString()) == "content" && value.IsString())
                {
                    // 如果是 content 字段，尝试对其内容进行二次解析
                    std::string cleanedContent = cleanNestedJson(value.GetString());
                    auto nestedResult          = parseNestedJson(cleanedContent);
                    result.insert(nestedResult.begin(), nestedResult.end());
                }
                else
                {
                    result[key.GetString()] = convertValueToString(value);
                }
            }
        }

        return result;
    }

    std::string cleanNestedJson(const std::string &nestedJsonString)
    {
        // 去掉无效前缀，例如 "/1.0/"
        std::string cleanedString = std::regex_replace(nestedJsonString, std::regex("^/[^/]+/"), "");

        // 替换单引号为双引号
        cleanedString = std::regex_replace(cleanedString, std::regex("'"), "\"");

        return cleanedString;
    }

    std::unordered_map<std::string, std::string> parseNestedJson(const std::string &nestedJsonString)
    {
        std::unordered_map<std::string, std::string> result;

        rapidjson::Document nestedDocument;
        if (nestedDocument.Parse(nestedJsonString.c_str()).HasParseError())
        {
            std::cerr << "Nested JSON parse error: " << rapidjson::GetParseError_En(nestedDocument.GetParseError())
                      << std::endl;
            return result;
        }

        if (nestedDocument.IsObject())
        {
            for (const auto &[key, value] : nestedDocument.GetObject())
            {
                result[key.GetString()] = convertValueToString(value);
            }
        }

        return result;
    }

    template <typename T>
    std::enable_if_t<std::is_arithmetic_v<T>, std::string> to_string(T value) const
    {
        return std::to_string(value);
    }

    std::string convertValueToString(const rapidjson::Value &value) const
    {
        if (value.IsString())
        {
            return value.GetString();
        }
        else if (value.IsInt())
        {
            return to_string(value.GetInt());
        }
        else if (value.IsInt64())
        {
            return to_string(value.GetInt64());
        }
        else if (value.IsDouble())
        {
            return to_string(value.GetDouble());
        }
        else if (value.IsBool())
        {
            return value.GetBool() ? "true" : "false";
        }
        return ""; // 对于其他类型，返回空字符串
    }
};
