#include <eosio/epn_plugin/epn_plugin.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>

namespace eosio {
   static appbase::abstract_plugin& _epn_plugin = app().register_plugin<epn_plugin>();

    class epn_plugin_impl {
    public:
    };

    epn_plugin::epn_plugin():my(new epn_plugin_impl()){}
    epn_plugin::~epn_plugin(){}

    void epn_plugin::set_program_options(options_description&, options_description& cfg) {

        // cfg.add_options()
        //         ("option-name", bpo::value<string>()->default_value("default value"), "Option Description")
        //     ;
    }

    void epn_plugin::plugin_initialize(const variables_map& options) {
        try {
            //if( options.count( "option-name" )) {
                // Handle the option
            //}
        } FC_LOG_AND_RETHROW() 
    }

    void epn_plugin::plugin_startup() {
        ilog("EOS Power Network plugin:  plugin_startup() begin");
        try{
            // auto& chain = app().find_plugin<chain_plugin>()->chain();
            // my->accepted_block_conn.emplace(chain.accepted_block.connect(
            //     [&](const block_state_ptr& b_state) {
            //         my->on_accepted_block(b_state);
            // }));
            // my->send_heartbeat_transaction();
        }
        FC_LOG_AND_DROP();
    }

    void epn_plugin::plugin_shutdown() {
    // OK, that's enough magic
    }

}
