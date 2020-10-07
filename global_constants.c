/*-----------------------------------------------------------------------------
Compile-time constant integers.
-----------------------------------------------------------------------------*/
enum { PRNG_BUF_LENGTH    =   256 };                           // pseudorandom noise generator buffer size
enum { BACKTRACE_ARR_SIZE =     8 };                           // segmentation fault backtrace depth
enum { NUM_OF_HASHES      = 65535 };                           // number of times passphrase is hashed
enum { AES_KEY_LENGTH     =    32 };                           // AES encryption key length (bytes)
enum { INIT_VEC_LENGTH    =    16 };                           // AES initialisation vector length (bytes)
enum { TOAST_TIMEOUT      =     5 * G_TIME_SPAN_MILLISECOND }; // time for which toast is shown
enum { ENTRY_WIDTH        =    40 };                           // GTK entry width in characters

/*-----------------------------------------------------------------------------
Paths to the icons used in the application.
-----------------------------------------------------------------------------*/
char const *const icon_main = "./include/icons/favicon.png";
char const *const icon_vis  = "./include/icons/visible.png";
char const *const icon_warn = "./include/icons/warning.png";
char const *const icon_edit = "./include/icons/edit.png";
char const *const icon_del  = "./include/icons/delete.png";

/*-----------------------------------------------------------------------------
Plain strings.
-----------------------------------------------------------------------------*/
char const *const str_title = "Password Manager";
char const *const str_passphrase = "Passphrase";
char const *const str_passphrase_toggle = "Click to show or hide passphrase.";
char const *const str_passphrase_submit = "Submit";

/*-----------------------------------------------------------------------------
Strings formatted using Pango (which is the markup language GTK understands).
-----------------------------------------------------------------------------*/
char const *const msg_passphrase      = "<span weight=\"bold\">Enter your passphrase to log in.</span>";
char const *const msg_manage          = "<span weight=\"normal\">Manage Passwords</span>";
char const *const msg_manage_header   = "<span weight=\"bold\">Type into the search box to display matching items.</span>";
char const *const msg_manage_website  = "<span weight=\"bold\">                      Website                      </span>";
char const *const msg_manage_username = "<span weight=\"bold\">                      Username                      </span>";
char const *const msg_manage_password = "<span weight=\"bold\">                      Password                      </span>";
char const *const msg_manage_error    = "<span weight=\"bold\">No Matching Items</span>";
char const *const msg_add             = "<span weight=\"normal\">Add New Password</span>";
char const *const msg_add_header      = "<span weight=\"bold\">Fill these fields to add a new password.</span>";
char const *const msg_change          = "<span weight=\"normal\">Change Passphrase</span>";
char const *const msg_change_header   = "<span weight=\"bold\">Fill these fields to change the passphrase.</span>";

/*-----------------------------------------------------------------------------
File names. The first two are the names of the files which contain the
passphrase and the passwords respectively. The other two are the names which
will be used to create temporary files when the contents of the original files
have to be changed.
-----------------------------------------------------------------------------*/
char const *const Master   = ".Master.bin";
char const *const Slave    = ".Slave.bin";
char const *const Master__ = ".Master.tmp";
char const *const Slave__  = ".Slave.tmp";

