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
Title and label strings.
-----------------------------------------------------------------------------*/
char const *const str_title        = "Password Manager";
char const *const str_passphrase   = "Passphrase";
char const *const str_submit       = "Submit";
char const *const str_confirmation = "Confirmation";
char const *const str_accept       = "Yes";
char const *const str_reject       = "No";
char const *const str_search       = "Search";
char const *const str_placeholder  = "...";
char const *const str_website      = "Website";
char const *const str_website_hint = "e.g. Twitter, ICICI, Google";
char const *const str_username     = "Username";
char const *const str_password1    = "Password";
char const *const str_password2    = "Confirm Password";
char const *const str_passphrase1  = "New Passphrase";
char const *const str_passphrase2  = "Confirm New Passphrase";
char const *const str_password_gen = "Generate Random Password";
char const *const str_edit         = "Update";
char const *const str_add          = "Add";
char const *const str_change       = "Change";

/*-----------------------------------------------------------------------------
Tooltip strings.
-----------------------------------------------------------------------------*/
char const *const str_toggle_passphrase      = "Click to show or hide passphrase.";
char const *const str_wrong_passphrase       = "Cannot log in. Wrong passphrase entered.";
char const *const str_show_password          = "Click to show password.";
char const *const str_toggle_password        = "Click to show or hide password.";
char const *const str_suggest_password       = "Click to suggest a strong password.";
char const *const str_edit_password          = "Click to update this item.";
char const *const str_edit_err1              = "Cannot update item. One or more fields are empty.";
char const *const str_edit_err2              = "Cannot update item. Fields \'Password\' and \'Confirm Password\' do not match.";
char const *const str_edit_password_done     = "Updated successfully.";
char const *const str_delete_password        = "Click to delete this item.";
char const *const str_delete_password_done   = "Deleted successfully.";
char const *const str_add_err1               = "Cannot add password. One or more fields are empty.";
char const *const str_add_err2               = "Cannot add password. Fields \'Password\' and \'Confirm Password\' do not match.";
char const *const str_add_password_done      = "Added successfully.";
char const *const str_change_passphrase_err1 = "Cannot change passphrase. One or both of the fields are empty.";
char const *const str_change_passphrase_err2 = "Cannot change passphrase. Fields \'New Passphrase\' and \'Confirm New Passphrase\' do not match.";
char const *const str_change_passphrase_done = "Changed successfully.";

/*-----------------------------------------------------------------------------
Dialogue box label strings.
-----------------------------------------------------------------------------*/
char const *const str_delete_password_question   = "Are you sure you want to delete this item? This action is irreversible.";
char const *const str_edit_password_question     = "Are you sure you want to update this item?";
char const *const str_add_password_question      = "Are you sure you want to add this item?";
char const *const str_change_passphrase_question = "Are you sure you want to change the passphrase? If you forget the passphrase, your passwords will become permanently unrecoverable.";

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
char const *const msg_edit_header     = "<span weight=\"bold\">Fill these fields to update this entry.</span>";
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

