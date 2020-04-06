
const int FILE_APPEND_DATA = 4;
const int OPEN_ALWAYS = 4;
const string LOGFILE_PATH = "System/DII/log.txt";
const int LOG_INFOS = 0;
const int LOG_WARNINGS = 1;
const int LOG_ERRORS = 0;


func int BW_AppendTo(var string file) {
    _bin_prefix = "BW_NewFile";
    if(!_BIN_nRunning()) { return 0; };

    _bin_open = WIN_CreateFile(file, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(_bin_open==-1) {
        _bin_open = 0;
        var string err; err = ConcatStrings(file, " - Datei konnte nicht erstellt werden. Fehlercode ");
        _BIN_Err(ConcatStrings(err, IntToString(WIN_GetLastError())));
        return 0;
    };

    if(!_bin_ccnt) {
        _bin_clen = _BIN_BufferLength;
        _bin_ccnt = MEM_Alloc(_bin_clen);
    };

    _bin_mode = 0;
    _bin_crsr = 0;
    return 1;
};




func void logInfo(var string msg) {
    
    if (!LOG_INFOS) {
        return;
    };

    if (!BW_AppendTo(LOGFILE_PATH))
    {
        return;
    };
    
    BW_Text("[Info] ");
    BW_Text(msg);
    BW_NextLine();
    BW_Close();
    
};

func void logWarn(var string msg) {
    if (!LOG_WARNINGS) {
        return;
    };

    if (!BW_AppendTo(LOGFILE_PATH))
    {
        return;
    };
    
    BW_Text("[Warn] ");
    BW_Text(msg);
    BW_NextLine();
    BW_Close();
};

func void logError(var string msg) {
    if (!LOG_ERRORS) {
        return;
    };
    
    if (!BW_AppendTo(LOGFILE_PATH))
    {
        return;
    };
    
    BW_Text("[Error] ");
    BW_Text(msg);
    BW_NextLine();
    BW_Close();  
};