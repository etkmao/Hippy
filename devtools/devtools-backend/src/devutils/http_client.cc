/*
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2017-2019 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "devutils/http_client.h"
#include "footstone/logging.h"
#include <asio/ip/tcp.hpp>
#include <iostream>
#include "tunnel/ws/web_socket_channel.h"

namespace hippy::devtools {

using asio::ip::tcp;

void HttpClient::Connect(std::string &body) {
  try
  {
    asio::ip::tcp::iostream s;

    // The entire sequence of I/O operations must complete within 60 seconds.
    // If an expiry occurs, the socket is automatically closed and the stream
    // becomes bad.
    s.expires_after(asio::chrono::seconds(60));

    // Establish a connection to the server.
    s.connect("localhost", "9225");
    if (!s) {
      std::cout << "Unable to connect: " << s.error().message() << "\n";
      return;
    }

    // Send the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    s << "GET " << "/json" << " HTTP/1.0\r\n";
    s << "Host: " << "localhost" << "\r\n";
    s << "Accept: */*\r\n";
    s << "Connection: close\r\n\r\n";

    // By default, the stream is tied with itself. This means that the stream
    // automatically flush the buffered output before attempting a read. It is
    // not necessary not explicitly flush the stream at this point.

    // Check that response is OK.
    std::string http_version;
    s >> http_version;
    unsigned int status_code;
    s >> status_code;
    std::string status_message;
    std::getline(s, status_message);
    if (!s || http_version.substr(0, 5) != "HTTP/") {
      std::cout << "Invalid response\n";
      return;
    }
    if (status_code != 200) {
      std::cout << "Response returned with status code " << status_code << "\n";
      return;
    }

    // Process the response headers, which are terminated by a blank line.
    std::string header;
    while (std::getline(s, header) && header != "\r") {
      FOOTSTONE_LOG(INFO) << "xxx hippy, http header: " << header;
    }
    FOOTSTONE_LOG(INFO) << "xxx hippy, http header end.";

    std::string bodyLine;
    while (std::getline(s, bodyLine) && bodyLine != "\n" && bodyLine != "") {
      FOOTSTONE_LOG(INFO) << "xxx hippy, http body: " << bodyLine;
      body += bodyLine;
    }
    FOOTSTONE_LOG(INFO) << "xxx hippy, http body end.";
    // Write the remaining data to output.
    // std::cout << s.rdbuf();
  }
  catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << "\n";
  }
}

} // namespace hippy::devtools
