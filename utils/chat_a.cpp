// An experiment to implement a simple chat program to understand NDN-CXX API.

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <string>
#include <iostream>

using namespace ndn;

class Chat {
  public:
  Chat(std::string prefix1, std::string prefix2)
    : my_prefix(prefix1), friend_prefix(prefix2) {}

  void run() {
    face.setInterestFilter(
      my_prefix,
      std::bind(&Chat::on_interest, this, _1, _2),
      nullptr,
      std::bind(&Chat::on_register_failed, this, _1, _2)
    );

    face.processEvents();
  }

  private:
  void on_interest(const InterestFilter&, const Interest& interest) {
    std::string message;
    std::cout << ">> ";
    std::cin >> message;

    auto data = make_shared<Data>(interest.getName());
    data->setFreshnessPeriod(10_s);
    data->setContent(reinterpret_cast<const uint8_t*>(message.data()), message.size());

    keychain.sign(*data);
    face.put(*data);

    express_interest_for_response();
  }

  void on_register_failed(const Name& prefix, const std::string& reason) {
    std::cerr << "ERROR: Failed to register prefix '"
      << prefix << "' with the local forwarder (" << reason << ")"
      << std::endl;
    face.shutdown();
  }

  void express_interest_for_response() {
    Name interestName(friend_prefix);
    interestName.appendVersion();

    Interest interest(interestName);
    interest.setCanBePrefix(false);
    interest.setMustBeFresh(true);
    interest.setInterestLifetime(100_s);

    face.expressInterest(
      interest,
      bind(&Chat::on_response, this,  _1, _2),
      [](const Interest&, const lp::Nack& nack) {
        std::cout << "Received Nack with reason " << nack.getReason() << std::endl;
      },
      [](const Interest& interest) {
        std::cout << "Timeout for " << interest << std::endl;
      });
  }

  void on_response(const Interest&, const Data& data) {
    std::string response((char*)data.getContent().value(), data.getContent().value_size());
    std::cout << response << '\n';
  }
  private:
    Face face;
    KeyChain keychain;
    std::string my_prefix;
    std::string friend_prefix;
};

int main(int argc, char* argv[]) {
  Chat chat("/chatroom/a/", "/chatroom/b/");
  chat.run();
  return 0;
}
