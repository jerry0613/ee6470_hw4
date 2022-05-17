#ifndef GB_FILTER_H_
#define GB_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "GBfilter_def.h"

struct GBFilter : public sc_module {
  tlm_utils::simple_target_socket<GBFilter> tsock;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<unsigned char> o_result_r;
  sc_fifo<unsigned char> o_result_g;
  sc_fifo<unsigned char> o_result_b;

  SC_HAS_PROCESS(GBFilter);

  GBFilter(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &GBFilter::blocking_transport);
    SC_THREAD(do_filter);
  }

  ~GBFilter() {
	}

  int val_r, val_g, val_b;
  unsigned char r, g, b;
  unsigned int base_offset;

  void do_filter(){
    { wait(CLOCK_PERIOD, SC_NS); }
    while (true) {
      {
        val_r = 0;
        val_g = 0;
        val_b = 0;
        wait(CLOCK_PERIOD, SC_NS);
      }
      for (unsigned int v = 0; v < MASK_Y; ++v) {
        for (unsigned int u = 0; u < MASK_X; ++u) {
          r = i_r.read();
          g = i_g.read();
          b = i_b.read();
          wait(CLOCK_PERIOD, SC_NS);
          {
            val_r += r * GBmask[u][v];
            val_g += g * GBmask[u][v];
            val_b += b * GBmask[u][v];
            wait(CLOCK_PERIOD, SC_NS);
          }
        }
      }

      o_result_r.write(val_r/16);
      o_result_g.write(val_g/16);
      o_result_b.write(val_b/16);
    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;

    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case GB_FILTER_RESULT_ADDR:
            buffer.uc[0] = o_result_r.read();
            buffer.uc[1] = o_result_g.read();
            buffer.uc[2] = o_result_b.read();
            break;
          default:
            std::cerr << "READ Error! GBFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
          }
        data_ptr[0] = buffer.uc[0];
        data_ptr[1] = buffer.uc[1];
        data_ptr[2] = buffer.uc[2];
        data_ptr[3] = buffer.uc[3];
        break;
      case tlm::TLM_WRITE_COMMAND:
        // cout << "WRITE" << endl;
        switch (addr) {
          case GB_FILTER_R_ADDR:
            i_r.write(data_ptr[0]);
            i_g.write(data_ptr[1]);
            i_b.write(data_ptr[2]);
            break;
          default:
            std::cerr << "WRITE Error! GBFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
        }
        break;
      case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      }
      payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
  }
};
#endif
