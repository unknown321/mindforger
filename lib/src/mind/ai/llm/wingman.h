/*
 wingman.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef M8R_WINGMAN_H
#define M8R_WINGMAN_H

#include <string>
#include <vector>

#include "../../../debug.h"
#include "../../../config/configuration.h"

namespace m8r {

/*
 * Predefined LLM prompts.
 */

constexpr const auto CTX_INCLUDE_NAME = "#NAME";
constexpr const auto CTX_INCLUDE_TEXT = "#TEXT";

constexpr const auto PROMPT_SUMMARIZE = "Summarize: #NAME. #TEXT";
constexpr const auto PROMPT_GENERATE_TAGS = "Generate tags for: #NAME. #TEXT";
constexpr const auto PROMPT_FIND_TASKS = "Find tasks in: #NAME. #TEXT";
constexpr const auto PROMPT_FIND_PERSONS = "Find persons names in: #NAME. #TEXT";
constexpr const auto PROMPT_FIND_LOCATIONS = "Find locations in: #NAME. #TEXT";
constexpr const auto PROMPT_FIND_ORGS = "Find organizations in: #NAME. #TEXT";
constexpr const auto PROMPT_CHAT = "Chat with the context.";

constexpr const auto PROMPT_SHORTEN = "Shorten: #TEXT";
constexpr const auto PROMPT_EXPLAIN_LIKE_5 = "Explain like I'm 5: #NAME";
constexpr const auto PROMPT_EXPLAIN_LIKE_5_TXT = "Explain like I'm 5: #TEXT";
constexpr const auto PROMPT_FIX_GRAMMAR = "Fix grammar in: #TEXT";
constexpr const auto PROMPT_FIND_GRAMMAR = "Find grammar errors in: #TEXT";
constexpr const auto PROMPT_TRANSLATE_EN = "Translate to English: #TEXT";
constexpr const auto PROMPT_REWRITE_FORMALLY = "Rewrite formally: #TEXT";
constexpr const auto PROMPT_REWRITE_ACADEMIA = "Rewrite in academic style: #TEXT";
constexpr const auto PROMPT_REWRITE_SLANG = "Rewrite in a slang style: #TEXT";
constexpr const auto PROMPT_SYNONYM = "Give synonym to: #TEXT";
constexpr const auto PROMPT_ANTONYM = "Give antonym to: #TEXT";
constexpr const auto PROMPT_COMPLETE_TEXT = "Complete work-in-progress text: #TEXT";

// other UCs:
// - NER UCs
// - simplify
// - beautify
// - translate
// - fix spelling
// - fix style
// - create plan ...

// Wingman provider service status codes
enum WingmanStatusCode {
    WINGMAN_STATUS_CODE_OK,
    WINGMAN_STATUS_CODE_ERROR
};

/**
 * Wingman chat request command pattern must be used as asynchronous requests.
 * As it cannot handle that many parameters this structure is used.
 */
struct CommandWingmanChat {
    std::string prompt;
    std::string httpResponse;
    WingmanStatusCode status;
    std::string errorMessage;
    std::string answerLlmModel;
    int promptTokens;
    int answerTokens;
    std::string answerMarkdown;
};


/**
 * Wingman is a class that provides a set of LLM-based use cases.
 */
class Wingman
{
private:
    WingmanLlmProviders llmProvider;

    std::vector<std::string> outlinePrompts = {
        PROMPT_SUMMARIZE,
        PROMPT_TRANSLATE_EN,
        PROMPT_FIND_GRAMMAR,
        PROMPT_GENERATE_TAGS,
        PROMPT_FIND_TASKS,
        PROMPT_FIND_PERSONS,
        PROMPT_FIND_LOCATIONS,
        PROMPT_FIND_ORGS,
        // PROMPT_CHAT,
    };

    std::vector<std::string> notePrompts = {
        PROMPT_SUMMARIZE,
        PROMPT_SHORTEN,
        PROMPT_FIND_GRAMMAR,
        PROMPT_EXPLAIN_LIKE_5,
        PROMPT_TRANSLATE_EN,
        PROMPT_FIND_TASKS,
        PROMPT_GENERATE_TAGS,
        PROMPT_FIX_GRAMMAR,
        PROMPT_REWRITE_FORMALLY,
        PROMPT_REWRITE_SLANG,
        // PROMPT_CHAT,
    };

    std::vector<std::string> textPrompts = {
        PROMPT_COMPLETE_TEXT,
        PROMPT_EXPLAIN_LIKE_5_TXT,
        PROMPT_FIX_GRAMMAR,
        PROMPT_FIND_GRAMMAR,
        PROMPT_TRANSLATE_EN,
        PROMPT_GENERATE_TAGS,
        PROMPT_REWRITE_FORMALLY,
        PROMPT_REWRITE_ACADEMIA,
        PROMPT_REWRITE_SLANG,
        PROMPT_SYNONYM,
        PROMPT_ANTONYM,
    };

public:
    explicit Wingman(WingmanLlmProviders llmProvider);
    Wingman(const Wingman&) = delete;
    Wingman(const Wingman&&) = delete;
    Wingman& operator =(const Wingman&) = delete;
    Wingman& operator =(const Wingman&&) = delete;
    virtual ~Wingman();

    virtual const std::vector<std::string>& getPredefinedOPrompts() {
        return outlinePrompts;
    }
    virtual const std::vector<std::string>& getPredefinedNPrompts() {
        return notePrompts;
    }
    virtual const std::vector<std::string>& getPredefinedTPrompts() {
        return textPrompts;
    }

    virtual void chat(CommandWingmanChat& command) = 0;
};

}
#endif // M8R_WINGMAN_H
