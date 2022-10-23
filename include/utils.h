#ifndef UTILS
#define UTILS

// #ifdef __cplusplus
// extern "C" {
// #endif


/**
 * @brief Closes a preciously opened port.
 * 
 * @param port_fd 
 * @return int 
 */
int close_port(int port_fd);

/**
 * @brief Configues the baud, party, data bits, and stop bits of a port.
 * 
 * @param port 
 * @param baud 
 * @param parity 
 * @param data_bit 
 * @param stop_bits 
 * @return int 
 */
int open_port(const char *port, int baud, char parity, int data_bit, int stop_bits);

/**
 * @brief Sets the vmin and vtime on the port affecting whether it blocking when reading.
 * If VMIN is greater than 1:
 *      Read calls will block until any data is recieved.
 *      VTIME will configure the timeout from after the first byte recieved.
 *      This can cause an indefinite blocking call if data is never recieved.
 * If VMIN is 0:
 *      VTIME will configure the timeout for waiting when the first byte recieved.
 *      A VTIME of 0 will create an non blocking port.
 * 
 * @param port 
 * @param vmin 
 * @param vtime 
 * @return int 
 */
int set_port_synchronization(const char *port, int vmin, int vtime);

// #ifdef __cplusplus
// }
// #endif

#endif /* UTILS */
