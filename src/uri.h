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

    [[nodiscard]] std::string_view getBody() const;

    [[nodiscard]] std::string_view getRawUri() const;

private:
    Uri() = default;

private:
    std::string QueryString;
    std::string Path;
    std::string Protocol;
    std::string Host;
    std::string RawUri;
    std::string Body;
    int Port = 0;
};

#endif //CLASHSUBGENERATOR_URI_H
