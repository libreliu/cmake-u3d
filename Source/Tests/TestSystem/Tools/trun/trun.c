#***************************************************************************
#
#  Copyright (c) 2003 - 2006 Intel Corporation
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#***************************************************************************

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int child_deleted = 0;
int finished = 0;
int was_assertion = 0;
/*
----------------------------------------------------------------------------------------------------

Function prints the message to standard error stream and terminates the program.

----------------------------------------------------------------------------------------------------
*/

void die(
    char const *    message,
    ...
) {

  va_list args;

  va_start( args, message );
  fprintf( stderr, "trun: " );
  vfprintf( stderr, message, args );
  fprintf( stderr, "\n" );
  va_end( args );
  exit( EXIT_FAILURE );

}; /* function die */



/*
----------------------------------------------------------------------------------------------------

Function prints the message to standard output stream.

----------------------------------------------------------------------------------------------------
*/

void say(
    char const *    message,
    ...
) {

  va_list args;

  va_start( args, message );
  fprintf( stdout, "\ntrun: " );
  vfprintf( stdout, message, args );
  fprintf( stdout, "\n" );
  fflush( stdout );
  va_end( args );

}; /* function say */



/*
----------------------------------------------------------------------------------------------------

Function parses the argument in the command line.

----------------------------------------------------------------------------------------------------
*/

void parse_argument(
    char ** line,       /* M: The command line to parse.         */
    char *  argument,     /* O: The buffer receiving the argument. */
    int     len           /* I: The size of argument buffer.       */
) {

    enum mode_t {
        norm,
        space,
        quote,
        escape
    }; /* enum mode_t */

    enum mode_t  mode;
    enum mode_t  prev_mode;

    #define ch()        ( * * line )
    #define skip()      { ++ ( * line ); }
    #define copy()      { put( ch() ); skip(); }
    #define put( c )                                                                            \
        {                                                                                       \
            if ( len > 0 ) {                                                                    \
                * argument = c;                                                                 \
                ++ argument;                                                                    \
                -- len;                                                                         \
            } else {                                                                            \
                die( "Internal error: burref overflowed in the function parse_argument." );     \
            };                                                                                  \
        }

    mode = norm;
    for ( ; ; ) {
        switch ( mode ) {
            case norm : {
                switch ( ch() ) {
                    case 0 : {
                        goto exit;
                    } break;
                    case ' ' :
                    case '\t' : {
                        skip();
                        mode = space;
                    } break;
                    case '"' : {
                        skip();
                        mode = quote;
                    } break;
                    case '\\' : {
                        skip();
                        prev_mode = mode;
                        mode = escape;
                    } break;
                    default : {
                        copy();
                    };
                }; /* switch ch() */
            } break; /* case norm */
            case space : {
                switch ( ch() ) {
                    case 0 : {
                        goto exit;
                    } break;
                    case ' ' :
                    case '\t' : {
                        skip();
                    } break;
                    default : {
                        goto exit;
                    };
                }; /* switch ch() */
            } break; /* case space */
            case quote : {
                switch ( ch() ) {
                    case 0 : {
                        goto exit;
                    } break;
                    case '"' : {
                        skip();
                        mode = norm;
                    } break;
                    case '\\' : {
                        skip();
                        prev_mode = mode;
                        mode = escape;
                    } break;
                    default : {
                        copy();
                    };
                }; /* switch ch() */
            } break; /* case quote */
            case escape : {
                switch ( ch() ) {
                    case '"' : {
                        copy();
                    } break;
                    case '\\' : {
                        copy();
                    } break;
                    default : {
                        put( '\\' );
                    };
                }; /* switch ch() */
                mode = prev_mode;
            } break; /* case escape */
        }; /* switch mode */
    }; /* forever */
    exit:
    put( 0 );

    #undef ch
    #undef skip
    #undef copy
    #undef put

}; /* function parse_argument */



/*
----------------------------------------------------------------------------------------------------

Function returns an error message for the error code error_code.

Warning: Function returns a pointer to the local static buffer, so each call destroys the result of
the previous call.

----------------------------------------------------------------------------------------------------
*/

char const * error_message(
    DWORD   error_code
) {

    static char buffer[ 8192 ];
    DWORD       rc;

    rc =
        FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error_code,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), /* Use default language. */
            buffer,
            sizeof( buffer ),
            NULL
        ); /* FormatMessage */
    if ( rc == 0 ) {
        return "*** Can not show the Windows error message because FormatMessage failed. ***";
    }; /* if */
    return buffer;

}; /* function error_message */



/*
----------------------------------------------------------------------------------------------------

Callback function used with the EnumWindows.

----------------------------------------------------------------------------------------------------
*/

BOOL CALLBACK EnumWindowsProc(
    HWND      hwnd,      /* I: Handle to parent window */
    LPARAM    lParam     /* I: Application-defined value (command-line) */
) {
    int            it;
    long           linf;
    static char    buffer[ 256 ];
    char *         cmdline;
    char *         tmpstr;
    HWND           hok;


    it = GetWindowText( hwnd, buffer, sizeof( buffer ) );
    if ( ! it ) {
        return TRUE;
    }; /* if */

    cmdline = ( char * )lParam;
    tmpstr = strchr( cmdline, ' ' );
    if ( tmpstr != NULL ) {
        *tmpstr = '\0';
    }; /* if */

    if ( it && strstr( buffer, cmdline ) != NULL ) {
        linf = GetWindowLong( hwnd, GWL_EXSTYLE );
        if ( ! linf ) {
            die( "Can not get window info. Windows reports: %s", error_message( GetLastError() ) );
        }; /* if */
        if ( linf & WS_EX_TOPMOST ) {
            hok = FindWindowEx( hwnd, NULL, "BUTTON", "OK" );
            if ( hok == NULL ) {
                say( "Can not get OK button handle. Windows reports: %s", error_message( GetLastError() ) );
            } else {
                if ( !SetForegroundWindow( hwnd ) ) {
                    SetActiveWindow( hwnd );
                }; /* if */
                SendMessage( hok, WM_LBUTTONDOWN, 0, 0 );
                SendMessage( hok, WM_LBUTTONUP, 0, 0 );
                return FALSE;
            }; /* if .. else */
        }; /* if */

    }; /* if */

    return TRUE;

}; /* function EnumWindowsProc */


/*
----------------------------------------------------------------------------------------------------

Function closes assertion boxes of the process
(Retry/Abort/Ignore and Yes/No)

----------------------------------------------------------------------------------------------------
*/

BOOL CALLBACK MyEnumChildWindowProc( HWND hwnd, LPARAM lParam )
{
  DWORD hp;
  DWORD ht = GetWindowThreadProcessId( hwnd, &hp );
  char title[100];
  char* ignore = NULL;
  char* no = NULL;
  
  if( lParam == ht)
  {
	GetWindowText( hwnd, title, 100 );
	ignore = strstr( title, "Ignore" );
	no = strstr( title, "Yes" );

	if ( ignore != NULL || no != NULL )
	{
		SendMessage( hwnd, WM_LBUTTONDOWN, MK_LBUTTON, 0 );
		SendMessage( hwnd, WM_LBUTTONUP, MK_LBUTTON, 0 );
		child_deleted = 1;
		was_assertion = 1;
	}
  }
  if( hwnd ) return TRUE;
  return FALSE;
}

/*
----------------------------------------------------------------------------------------------------

Function gets child windows for all top-level windows in system

----------------------------------------------------------------------------------------------------
*/

BOOL CALLBACK MyEnumWindowProc( HWND hwnd, LPARAM lParam )
{
  if( hwnd ) EnumChildWindows( hwnd, MyEnumChildWindowProc, lParam );
  if( hwnd ) return TRUE;
  return FALSE;
}

/*
----------------------------------------------------------------------------------------------------

Function closes all windows owned by certain process

----------------------------------------------------------------------------------------------------
*/

BOOL CALLBACK TCEnumWindowProc( HWND hwnd, LPARAM lParam )
{
  DWORD hp;
  DWORD ht = GetWindowThreadProcessId( hwnd, &hp );
  if( lParam == ht )
  {
    SendMessage( hwnd, WM_CLOSE, 0, 0 );
  }
  if( hwnd ) return TRUE;
  return FALSE;
}

/*
----------------------------------------------------------------------------------------------------

Function attempts to close the process closing all its windows

----------------------------------------------------------------------------------------------------
*/

int TryClosing( PROCESS_INFORMATION process_info, DWORD timeout )
{
  DWORD exit_code = 0;
  BOOL brc;
  EnumWindows( TCEnumWindowProc, process_info.dwThreadId );
  WaitForSingleObject( process_info.hProcess, timeout );
  brc = GetExitCodeProcess( process_info.hProcess, &exit_code );
//  brc = GetExitCodeProcess( process_info.hProcess, &exit_code );
  if ( ! brc ) {
    die(
      "Can not get process exit code. Windows reports: %s",
      error_message( GetLastError() )
      ); /* die */
  }; /* if */
  if ( exit_code == STILL_ACTIVE ) {
    /* Process is not finished yet. There is an error. */
    die(
      "Can not abort process. Windows reports: %s",
      error_message( ERROR_ACCESS_DENIED )
      ); /* die */
  } else {
    /* Process finished. */
    say( "Process finished with exit code %u.", exit_code );
  }; /* if */
  return 0;
}
/*
----------------------------------------------------------------------------------------------------

Function terminates the process

----------------------------------------------------------------------------------------------------
*/

void Terminate( PROCESS_INFORMATION process_info, DWORD drc, BOOL brc )
{
  brc = TerminateProcess( process_info.hProcess, 0 );
  if ( ! brc ) {
    /* Aborting failed. Has process finished? */
    if ( GetLastError() == ERROR_ACCESS_DENIED ) {
      DWORD   exit_code;
      brc = GetExitCodeProcess( process_info.hProcess, &exit_code );
      if ( ! brc ) {
        die(
          "Can not get process exit code. Windows reports: %s",
          error_message( GetLastError() )
          ); /* die */
      }; /* if */
      if ( exit_code == STILL_ACTIVE ) {
        /* Process is not finished yet. There is an error. */
        die(
          "Can not abort process. Windows reports: %s",
          error_message( ERROR_ACCESS_DENIED )
          ); /* die */
      } else {
        /* Process finished. */
        say( "Process finished with exit code %u.", exit_code );
      }; /* if */
    } else {
      die(
        "Can not abort process. Windows reports: %s",
        error_message( GetLastError() )
        ); /* die */
    }; /* if */
  } else {
    /* Process is aborting. */
    //bw = EnumWindows(               /* Look for Application Error window */
    //    &EnumWindowsProc,           /* callback function */
    //    ( long )command_line        /* application-defined value */
    //); /* EnumWindows */
    drc = WaitForSingleObject( process_info.hProcess, INFINITE );
    if ( drc == WAIT_FAILED ) {
      die(
        "Can not abort process. Windows reports: %s",
        error_message( GetLastError() )
        ); /* die */
    }; /* if */
    say( "Time is out. Process aborted." );
  }; /* if */
}

/*
----------------------------------------------------------------------------------------------------

Function analyzes the result of process execution

----------------------------------------------------------------------------------------------------
*/

int AfterWait( PROCESS_INFORMATION process_info, DWORD drc, BOOL brc )
{
        switch ( drc ) {
            case WAIT_ABANDONED_0 : {
                /* WaitForSingleObject function failed. */
                die(
                    "Can not wait for terminating process. Windows reports: %s",
                    error_message( GetLastError() )
                ); /* die */
            } break;
            case WAIT_TIMEOUT : {
                /* Time is out. Try to abort process. */
        EnumWindows( MyEnumWindowProc, process_info.dwThreadId );
        return child_deleted;
            } break;
            case WAIT_OBJECT_0 : {
                /* Process finished. Try to get process exit code. */
                DWORD   exit_code;

				
                brc = GetExitCodeProcess( process_info.hProcess, &exit_code );
                if ( was_assertion && (exit_code == 0) )
					exit_code = 7;
				
				if ( ! brc ) {
                    die(
                        "Can not get process exit code. Windows reports: %s",
                        error_message( GetLastError() )
                    ); /* die */
                }; /* if */
                if ( exit_code == STILL_ACTIVE ) {
                    die(
                        "Unexpected Windows behaviour "
                            "(WaitForSingleObject functions reports process finished, "
                            "but GetExitCodeProcess reports process still active)."
                    );
                } else {
                    say( "Process finished with exit code %u.", exit_code );
          finished = 1;
                }; /* if */
            } break;
            default : {
                die( "Unexpected Windows behaviour (unexpected response from WaitForSingleObject function)." );
            };
        }; /* switch */
    return 0;
}

/*
----------------------------------------------------------------------------------------------------

Function executes the command line command_line. If process terminates before time is out, function
immediatelly returns. If is not terminated when time is out, function determines whether
the process has created assertion boxes. If yes, it closes them and gives the process
additional time. In other case the process is aborted.

----------------------------------------------------------------------------------------------------
*/

void timed_run(
    DWORD   timeout,        /* I: Timeout (in milliseconds). */
    char *  command_line    /* I: Command line to execute.   */
) {

   BOOL brc;
    STARTUPINFO             startup_info;
    PROCESS_INFORMATION     process_info;

    DWORD   drc;
  int waitres;

    startup_info.cb          = sizeof( startup_info );
    startup_info.lpReserved  = NULL;
    startup_info.lpDesktop   = NULL;
    startup_info.lpTitle     = NULL;
    startup_info.dwFlags     = 0;
    startup_info.cbReserved2 = 0;
    startup_info.lpReserved2 = NULL;

    process_info.hProcess = INVALID_HANDLE_VALUE;
    process_info.hThread  = INVALID_HANDLE_VALUE;

    __try {

        // SetErrorMode( SEM_NOGPFAULTERRORBOX );
        SetErrorMode( -1 );
        brc =
            CreateProcess(
                NULL,       /* Application name. */
                command_line,
                NULL,       /* Use default process security attributes. */
                NULL,       /* Use default thread security attributes. */
                TRUE,       /* Inherit handles. */
                0,      //CREATE_DEFAULT_ERROR_MODE, // 0
                NULL,       /* Use my environment. */
                NULL,       /* Use my current directory. */
                &startup_info,
                &process_info
            ); /* CreateProcess */
        if ( ! brc ) {
            die( "Can not create process. Windows reports: %s", error_message( GetLastError() ) );
        }; /* if */
    drc = WaitForSingleObject( process_info.hProcess, timeout );
    waitres = AfterWait( process_info, drc, brc );
    child_deleted = 0;
    while( waitres )
    {
      drc = WaitForSingleObject( process_info.hProcess, timeout );
      waitres = AfterWait( process_info, drc, brc );
      child_deleted = 0;
    }
    if( finished == 0 )
      if( TryClosing( process_info, timeout ) )
        Terminate( process_info, drc, brc );
    } __finally {

        if ( process_info.hThread != INVALID_HANDLE_VALUE ) {
            brc = CloseHandle( process_info.hThread );
            if ( ! brc ) {
                die(
                    "Can not close process thread handle. Windows reports: %s",
                    error_message( GetLastError() )
                ); /* die */
            }; /* if */
        }; /* if */

        if ( process_info.hProcess != INVALID_HANDLE_VALUE ) {
            brc = CloseHandle( process_info.hProcess );
            if ( ! brc ) {
                die(
                    "Can not close process handle. Windows reports: %s",
                    error_message( GetLastError() )
                ); /* die */
            }; /* if */
        }; /* if */

    }; /* __try */

}; /* function timed_run */



/*
----------------------------------------------------------------------------------------------------

Function main.

----------------------------------------------------------------------------------------------------
*/
int main() {

    char * command_line = GetCommandLine();
    DWORD  timeout;

    /* Parse command line. */
    {

        char *       argument = NULL;
        int const    arg_len  = (int)(strlen( command_line ) + 1);
        char         unit[ 3 ];
        char         tail[ 2 ];
        int          rc;

        __try {

            /* Allocate memory for argument. */
            argument = (char*)malloc( arg_len );
            if ( argument == NULL ) {
                die( "Can not allocate memory." );
            }; /* if */

            /* Skip myself name in the command line. */
            parse_argument( &command_line, argument, arg_len );

            /* Get first argument from the command line. */
            parse_argument( &command_line, argument, arg_len );
            if ( argument[ 0 ] == 0 ) {
                die( "Not enough arguments in the command line. (Use -h option to get help.)" );
            }; /* if */

            if ( ( strcmp( argument, "-h" ) == 0 ) || ( strcmp( argument, "-help" ) == 0 ) ) {
                printf( "Purpose:\n" );
                printf( "    Run a program and give it limited time for execution.\n" );
                printf( "\n" );
                printf( "Usage:\n" );
                printf( "    trun timeout program arguments...\n" );
                printf( "    trun -help\n" );
                printf( "\n" );
                printf( "    where \n" );
                printf( "        timeout   -- Time limit. Valid units are: ms, s (default), m, h.\n" );
                printf( "        program   -- Program to execute.\n" );
                printf( "        arguments -- Arguments to pass to program.\n" );
                printf( "\n" );
                printf( "Description:\n" );
                printf( "    trun starts program. If program terminates before time is\n" );
                printf( "    out, trun terminates too. If program is not terminated when time is out,\n" );
                printf( "    trun aborts the progtam.\n" );
                printf( "\n" );
                printf( "Examples:\n" );
                printf( "    trun 1 ...\n" );
                printf( "    trun 500ms cmd /c dir\n" );
                printf( "    trun 4h ...\n" );
                printf( "\n" );
                exit( EXIT_SUCCESS );
            }; /* if */

            unit[ 0 ] = 0;
            rc = sscanf( argument, "%d%2s%1s", &timeout, unit, tail );
            if ( ( rc < 1 ) || ( rc > 2 ) ) {
                die( "Can not parse timeout argument: \"%s\". (Use -h option to get help.)", argument );
            }; /* if */
            if ( timeout < 0 ) {
                die( "Timeout must not be negative: \"%s\". (Use -h option to get help.)", argument );
            }; /* if */

            /* Convert timeout to milliseconds. */
            if ( strcmp( unit, "ms" ) == 0 ) {
            } else if ( ( strcmp( unit, "" ) == 0 ) || ( strcmp( unit, "s" ) == 0 ) ) {
                timeout = timeout * 1000;
            } else if ( strcmp( unit, "m" ) == 0 ) {
                timeout = timeout * 60 * 1000;
            } else if ( strcmp( unit, "h" ) == 0 ) {
                timeout = timeout * 60 * 60 * 1000;
            } else {
                die( "Unknown unit of timeout: \"%s\". (Use -h option to get help.)", unit );
            };

        } __finally {

            free( argument );

        }; /* __try */

    };

    timed_run( timeout, command_line );

    exit( EXIT_SUCCESS );

} /* function main */


/* End of file. */
