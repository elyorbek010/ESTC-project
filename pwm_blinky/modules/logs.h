#ifndef LOGS_H
#define LOGS_H

/**
 * @brief Initialize logging over USB
 *
 * @param void no parameters needed
 */
void logs_init(void);

/**
 * @brief Send logs through USB
 *
 * @param void no parameters needed
 */
void send_logs(void);

#endif //LOGS_H