//
// Created by Kotarou on 2020/3/15.
//

#ifndef CLASHSUBGENERATOR_URI_H
#define CLASHSUBGENERATOR_URI_H

class Uri {
public:
    static Uri Parse(std::string_view uri);

    [[nodiscard]] std::string_view getQueryString() const;

    [[nodiscard]] std::string_view getPath() const;

    [[nodiscard]] std::string_view getProtocol() const;

    [[nodiscard]] std::string_view getHost() const;

    [[nodiscard]] int getPort() const;

private:
    Uri() = default;

    std::string QueryString;
    std::string Path;
    std::string Protocol;
    std::string Host;
    int Port = 0;
};

#endif //CLASHSUBGENERATOR_URI_H
