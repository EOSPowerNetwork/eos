#include <eosio/chain/contract_types.hpp>
#include <eosio/chain/name.hpp>
#include <eosio/chain/types.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/epn_plugin/epn_plugin.hpp>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace eosio {
static appbase::abstract_plugin &_epn_plugin =
    app().register_plugin<epn_plugin>();

using eosio::chain::name;

namespace {
void printBlockStateInfo(const chain::block_state_ptr &block_state) {
  ilog("\n\n--- Block state info ---\n");
  uint32_t block_num = block_state->header.block_num();
  size_t numTransactions = block_state->block->transactions.size();
  auto thisProducer = block_state->block->producer;
  const auto &prods = block_state->active_schedule.producers;
  auto blockID = block_state->id.str();

  ilog("Block ID: ${bid}", ("bid", blockID));
  ilog("Block number: ${bnum}", ("bnum", std::to_string(block_num)));
  ilog("Number of transactions in block: ${numtrans}\n",
       ("numtrans", std::to_string(numTransactions)));
  ilog("This producer: ${name}\n", ("name", thisProducer.to_string()));
  ilog("Producers: \n");
  for (const auto &prod : prods) {
    ilog("  - ${name}\n", ("name", prod.producer_name.to_string()));
  }
  ilog("\n");

  // ilog( "shutting down plugin ${name}", ("name",name()) );
  ilog("------------------------\n\n");
}
} // namespace

class epn_plugin_impl {
public:
  void accepted_block(const chain::block_state_ptr &block_state) {

    // Don't service any pull transactions if you're actively producing
    // if (producerName == block_state->block->producer.to_string())
    //  return;
  }

  std::array<fc::optional<boost::signals2::scoped_connection>, 1> connections;
  std::string permission;
  std::string producerName;
  fc::crypto::private_key signingPrivateKey;

private:
  static constexpr name contractName = N("exec.epn");
};

epn_plugin::epn_plugin() : my(new epn_plugin_impl()) {}
epn_plugin::~epn_plugin() {}

void epn_plugin::set_program_options(options_description &,
                                     options_description &cfg) {

  // clang-format off
      cfg.add_options()
         ("permission", bpo::value<string>(),
          "Permission name used for signing EPN transactions")
         ("operator-name", bpo::value<string>(),
          "Name of the registered EPN (producer) account signing for EPN transactions")
         ("signing-private-key", bpo::value<string>(),
          "Private key that satisfies the producer-name@permission authority")
         ;
  // clang-format on
}

void epn_plugin::plugin_initialize(const variables_map &options) {

  auto extractInto = [&](const std::string &paramName, std::string &target) {
    if (options.count(paramName)) {
      target = options.at(paramName).as<std::string>();
    }
  };

  try {
    extractInto("permission", my->permission);

    extractInto("operator-name", my->producerName);

    std::string prKey;
    extractInto("signing-private-key", prKey);

    if (my->permission.empty()) {
      throw std::runtime_error(
          "EPN plugin missing permission. See nodeos --help");
    }
    if (my->producerName.empty()) {
      throw std::runtime_error(
          "EPN plugin missing operator name. See nodeos --help");
    }
    if (prKey.empty()) {
      throw std::runtime_error(
          "EPN plugin missing private key. See nodeos --help");
    }

    my->signingPrivateKey = fc::crypto::private_key(prKey);
  }
  FC_LOG_AND_RETHROW()
}

#define CONNECT(NAME)                                                          \
  connect(chain.NAME, [&](const auto &param) { my->NAME(param); })

void epn_plugin::plugin_startup() {
  try {
    auto &chain = app().find_plugin<chain_plugin>()->chain();

    auto connect = [&, i = 0](auto &signal, auto lambda) mutable {
      my->connections.at(i).emplace(signal.connect(lambda));
      ++i;
    };

    CONNECT(accepted_block);
  }
  FC_LOG_AND_DROP();
}

void epn_plugin::plugin_shutdown() {
  for (auto &connection : my->connections) {
    connection.reset();
  }
}

} // namespace eosio
