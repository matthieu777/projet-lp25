#include "messages.h"
#include <sys/msg.h>
#include <string.h>

#include <stdio.h>

// Functions in this file are required for inter processes communication

/*!
 * @brief send_file_entry sends a file entry, with a given command code
 * @param msg_queue the MQ identifier through which to send the entry
 * @param recipient is the id of the recipient (as specified by mtype)
 * @param file_entry is a pointer to the entry to send (must be copied)
 * @param cmd_code is the cmd code to process the entry.
 * @return the result of the msgsnd function
 * Used by the specialized functions send_analyze*
 */
int send_file_entry(int msg_queue, int recipient, files_list_entry_t *file_entry, int cmd_code) {
    any_message_t msg; //utilisation du type defini 
    
    msg.list_entry.reply_to = msg_queue;//id d'un message
    msg.list_entry.mtype = recipient;//type de message
    msg.list_entry.payload = *file_entry; 
    msg.list_entry.op_code = cmd_code; 
    // au final cela utiliser la struct files_list_entry_transmit_t en passant par any_message_t  

    int result = msgsnd(msg_queue, &msg, sizeof(any_message_t) - sizeof(long), 0); //envoie du message
     if (result == -1) {    //gestion d'erreur
        printf("erreur\n");
    }

    return result; 
}

/*!
 * @brief send_analyze_dir_command sends a command to analyze a directory
 * @param msg_queue is the id of the MQ used to send the command
 * @param recipient is the recipient of the message (mtype)
 * @param target_dir is a string containing the path to the directory to analyze
 * @return the result of msgsnd
 */
int send_analyze_dir_command(int msg_queue, int recipient, char *target_dir) {

    any_message_t msg;
    msg.analyze_dir_command.mtype = recipient;   
    msg.analyze_dir_command.op_code = COMMAND_CODE_ANALYZE_DIR; //indique qu'il faut analyser le directory
    //cela reviens a utiliser la struct analyze_dir_command_t

    strncpy(msg.analyze_dir_command.target, target_dir, PATH_SIZE - 1);//copie du chemin
    msg.analyze_dir_command.target[PATH_SIZE - 1] = '\0'; //pour etre sur que le chemin ce fini 
   
    int result = msgsnd(msg_queue, &msg, sizeof(analyze_dir_command_t) - sizeof(long), 0); // envoie du message

    if (result == -1) {     //gestion d'erreur
        printf("erreur\n");
    }

    return result;
}


// The 3 following functions are one-liners

/*!
 * @brief send_analyze_file_command sends a file entry to be analyzed
 * @param msg_queue the MQ identifier through which to send the entry
 * @param recipient is the id of the recipient (as specified by mtype)
 * @param file_entry is a pointer to the entry to send (must be copied)
 * @return the result of the send_file_entry function
 * Calls send_file_entry function
 */
int send_analyze_file_command(int msg_queue, int recipient, files_list_entry_t *file_entry) {
    return send_file_entry(msg_queue, recipient, file_entry, COMMAND_CODE_ANALYZE_FILE); //COMMAND_CODE_ANALYZE_FILE indique qu'on doit analyser le fichier
}

/*!
 * @brief send_analyze_file_response sends a file entry after analyze
 * @param msg_queue the MQ identifier through which to send the entry
 * @param recipient is the id of the recipient (as specified by mtype)
 * @param file_entry is a pointer to the entry to send (must be copied)
 * @return the result of the send_file_entry function
 * Calls send_file_entry function
 */
int send_analyze_file_response(int msg_queue, int recipient, files_list_entry_t *file_entry) {
    return send_file_entry(msg_queue, recipient, file_entry, COMMAND_CODE_FILE_ANALYZED);  //COMMAND_CODE_FILE_ANALYZED indique que le ficheir envoyer est analyser
}

/*!
 * @brief send_files_list_element sends a files list entry from a complete files list
 * @param msg_queue the MQ identifier through which to send the entry
 * @param recipient is the id of the recipient (as specified by mtype)
 * @param file_entry is a pointer to the entry to send (must be copied)
 * @return the result of the send_file_entry function
 * Calls send_file_entry function
 */
int send_files_list_element(int msg_queue, int recipient, files_list_entry_t *file_entry) {
    return send_file_entry(msg_queue, recipient, file_entry, COMMAND_CODE_FILE_ENTRY ); //COMMAND_CODE_FILE_ENTRY  indique que'il s'agit d'un files list entry
}

/*!
 * @brief send_list_end sends the end of list message to the main process
 * @param msg_queue is the id of the MQ used to send the message
 * @param recipient is the destination of the message
 * @return the result of msgsnd
 */
int send_list_end(int msg_queue, int recipient) {
    any_message_t msg;
    msg.simple_command.mtype = recipient;
    msg.simple_command.message = COMMAND_CODE_LIST_COMPLETE;// indique que c'est la fin puisques la list est complete
    //reviens a utiliser la struct simple_command_t
    int result = msgsnd(msg_queue, &msg, sizeof(simple_command_t) - sizeof(long), 0);//envoie du message
    
    if (result == -1) {
        printf("erreur");   //cas d'echec
    }

    return result;
    
}

/*!
 * @brief send_terminate_command sends a terminate command to a child process so it stops
 * @param msg_queue is the MQ id used to send the command
 * @param recipient is the target of the terminate command
 * @return the result of msgsnd
 */
int send_terminate_command(int msg_queue, int recipient) {
    any_message_t msg;
    msg.simple_command.mtype = recipient;
    msg.simple_command.message = COMMAND_CODE_TERMINATE;//inquique que c'est un terminate command
    //reviens a utiliser la struct simple_command_t
    int result = msgsnd(msg_queue, &msg, sizeof(simple_command_t) - sizeof(long), 0);//envoie du message

    if (result == -1) {
        printf("erreur \n");    //cas d'erreur
    }

    return result;
}

/*!
 * @brief send_terminate_confirm sends a terminate confirmation from a child process to the requesting parent.
 * @param msg_queue is the id of the MQ used to send the message
 * @param recipient is the destination of the message
 * @return the result of msgsnd
 */
int send_terminate_confirm(int msg_queue, int recipient) {
    any_message_t msg;
    msg.simple_command.mtype = recipient;
    msg.simple_command.message = COMMAND_CODE_TERMINATE_OK;//indique que c'est un terminate confiramation

    int result = msgsnd(msg_queue, &msg, sizeof(simple_command_t) - sizeof(long), 0); //envoie message

    if (result == -1) {
        printf("erreur");   //cas d'erreur
    }

    return result;
}



