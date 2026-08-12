/*
  implementation of FILE_TRANSFER_PROTOCOL MAVLink sub-protocol
 */

#pragma once

#include "GCS_config.h"

#if AP_MAVLINK_FTP_ENABLED

#include "GCS.h"

#ifndef AP_MAVLINK_FTP_MAX_SESSIONS
#define AP_MAVLINK_FTP_MAX_SESSIONS 5
#endif

// size of the per-session read-ahead buffer used to batch filesystem
// reads during BurstReadFile/ReadFile. One filesystem read of this
// size is issued for every ~17 MAVLink packets served (at 239 B per
// packet), which is much more efficient than a FatFs call per packet.
#ifndef AP_MAVLINK_FTP_READAHEAD_SIZE
#define AP_MAVLINK_FTP_READAHEAD_SIZE 4096
#endif

class GCS_FTP {
public:
    static void handle_file_transfer_protocol(const mavlink_message_t &msg, mavlink_channel_t chan);
    static uint32_t get_last_send_ms(mavlink_channel_t chan);

private:
    enum class FTP_OP : uint8_t {
        None = MAV_FTP_OPCODE_NONE,
        TerminateSession = MAV_FTP_OPCODE_TERMINATESESSION,
        ResetSessions = MAV_FTP_OPCODE_RESETSESSION,
        ListDirectory = MAV_FTP_OPCODE_LISTDIRECTORY,
        OpenFileRO = MAV_FTP_OPCODE_OPENFILERO,
        ReadFile = MAV_FTP_OPCODE_READFILE,
        CreateFile = MAV_FTP_OPCODE_CREATEFILE,
        WriteFile = MAV_FTP_OPCODE_WRITEFILE,
        RemoveFile = MAV_FTP_OPCODE_REMOVEFILE,
        CreateDirectory = MAV_FTP_OPCODE_CREATEDIRECTORY,
        RemoveDirectory = MAV_FTP_OPCODE_REMOVEDIRECTORY,
        OpenFileWO = MAV_FTP_OPCODE_OPENFILEWO,
        TruncateFile = MAV_FTP_OPCODE_TRUNCATEFILE,
        Rename = MAV_FTP_OPCODE_RENAME,
        CalcFileCRC32 = MAV_FTP_OPCODE_CALCFILECRC,
        BurstReadFile = MAV_FTP_OPCODE_BURSTREADFILE,
        Ack = MAV_FTP_OPCODE_ACK,
        Nack = MAV_FTP_OPCODE_NAK,
    };

    enum class FTP_ERROR : uint8_t {
        None = MAV_FTP_ERR_NONE,
        Fail = MAV_FTP_ERR_FAIL,
        FailErrno = MAV_FTP_ERR_FAILERRNO,
        InvalidDataSize = MAV_FTP_ERR_INVALIDDATASIZE,
        InvalidSession = MAV_FTP_ERR_INVALIDSESSION,
        NoSessionsAvailable = MAV_FTP_ERR_NOSESSIONSAVAILABLE,
        EndOfFile = MAV_FTP_ERR_EOF,
        UnknownCommand = MAV_FTP_ERR_UNKNOWNCOMMAND,
        FileExists = MAV_FTP_ERR_FILEEXISTS,
        FileProtected = MAV_FTP_ERR_FILEPROTECTED,
        FileNotFound = MAV_FTP_ERR_FILENOTFOUND,
    };

    struct Transaction {
        uint32_t offset;
        mavlink_channel_t chan;        
        uint16_t seq_number;
        FTP_OP opcode;
        FTP_OP req_opcode;
        bool  burst_complete;
        uint8_t size;
        uint8_t session;
        uint8_t sysid;
        uint8_t compid;
        uint8_t data[239];
    };

    enum class FTP_FILE_MODE {
        Read,
        Write,
    };

    ObjectBuffer<Transaction> requests{AP_MAVLINK_FTP_MAX_SESSIONS};

    bool initialised;

    // session specific info
    class Session {
    public:
        int fd = -1;
        uint32_t last_send_ms;
        int16_t session_id;
        FTP_FILE_MODE mode; // work around AP_Filesystem not supporting file modes
        mavlink_channel_t chan;
        uint8_t sysid;
        uint8_t compid;

        bool check_name_len(const Transaction &request);
        int gen_dir_entry(char *dest, size_t space, const char * path, const struct dirent * entry); // FTP helper for emitting a dir response
        void list_dir(Transaction &request, Transaction &response);
        void push_reply(Transaction &reply);
        bool handle_request(Transaction &request, Transaction &reply);

        int close(void);

    private:
        // read-ahead buffer to amortise SD-card / FatFs overhead across
        // many small FTP packets.
        uint8_t  read_buf[AP_MAVLINK_FTP_READAHEAD_SIZE];
        uint32_t read_buf_file_offset;
        uint16_t read_buf_valid_bytes = 0;

        // read up to size bytes at the given file offset into dest, serving
        // from the read-ahead buffer when possible. Returns bytes read, 0
        // at EOF, or -1 on error (errno set). Assumes fd is valid and mode
        // is Read.
        ssize_t read_at(uint32_t offset, uint8_t *dest, uint16_t size);
    };
    Session sessions[AP_MAVLINK_FTP_MAX_SESSIONS];

    bool init(void);

    static bool send_reply(const Transaction &reply);
    static void error(Transaction &response, FTP_ERROR error);

    /*
      setup reply packet to reply to the request
     */
    void setup_reply(const Transaction &request, Transaction &reply);

    void worker(void);

    // GCS_FTP instance created by static handle_file_transfer_protocol()
    static GCS_FTP *ftp;
};

#endif  // AP_MAVLINK_FTP_ENABLED
