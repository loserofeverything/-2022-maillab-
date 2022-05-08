#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>
#include "base64_utils.h"

#define MAX_SIZE 4095

char buf[MAX_SIZE+1];


char* join(const char *s1, const char *s2)
{
char *result = malloc(strlen(s1)+strlen(s2)+1);
if (result == NULL) exit (1);

strcpy(result, s1);
strcat(result, s2);

return result;
}

void testPrint(const char * input){
    printf("client : %s", input);
}

// receiver: mail address of the recipient
// subject: mail subject
// msg: content of mail body or path to the file containing mail body
// att_path: path to the attachment
void send_mail(const char* receiver, const char* subject, const char* msg, const char* att_path)
{
    const char* end_msg = "\r\n.\r\n";
    const char * end_command = "\r\n";
    const char* host_name = "smtp.qq.com"; // TODO: Specify the mail server domain name
    const unsigned short port = 25; // SMTP server port
    const char* user = "xxxxxxxx@qq.com"; // TODO: Specify the user
    const char* pass = "yyyyyyyy"; // TODO: Specify the password
    const char* from = "xxxxxxxx@qq.com"; // TODO: Specify the mail address of the sender
    char dest_ip[16]; // Mail server IP address
    int s_fd; // socket file descriptor
    struct hostent *host;
    struct in_addr **addr_list;
    int i = 0;
    int r_size;
    const char * boundary = "--wzyyydswzyyydswzyyyds\r\n";
    const char * MIME = "Content-Type: multipart/mixed; boundary=wzyyydswzyyydswzyyyds\n";

    // Get IP from domain name
    if ((host = gethostbyname(host_name)) == NULL)
    {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    addr_list = (struct in_addr **) host->h_addr_list;
    while (addr_list[i] != NULL)
        ++i;
    strcpy(dest_ip, inet_ntoa(*addr_list[i-1]));

    // TODO: Create a socket, return the file descriptor to s_fd, and establish a TCP connection to the mail server

    //Create a socket, return the file descriptor to s_fd
    s_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(dest_ip);
    serv_addr.sin_port = htons(25);
    //create connection
    connect(s_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));


    // Print welcome message
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);

    // Send EHLO command and print server response
    const char* EHLO = "ehlo qq.com\r\n"; // TODO: Enter EHLO command here
    testPrint(EHLO);
    send(s_fd, EHLO, strlen(EHLO), 0);
    // TODO: Print server response to EHLO command
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // TODO: Authentication. Server response should be printed out.
    const char* Auth_login = "auth login\r\n";
    testPrint(Auth_login);
    send(s_fd, Auth_login, strlen(Auth_login), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    // TODO: Send MAIL FROM command and print server response

    char * temp = encode_str(user);
    const char * Cipher_username = join(temp, end_command);
    testPrint(Cipher_username);
    send(s_fd, Cipher_username, strlen(Cipher_username), 0);
    free(temp);

    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);

    temp = encode_str(pass);
    const char * Verify_code = join(temp, end_command);
    testPrint(Verify_code);
    send(s_fd, Verify_code, strlen(Verify_code), 0);
    free(temp);

    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);

    const char* Mail_message = join(join(join(join("MAIL FROM:","<"),from),">"), end_command);
    testPrint(Mail_message);
    send(s_fd, Mail_message, strlen(Mail_message), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    
    // TODO: Send RCPT TO command and print server response
    const char* Rcpt_message = join(join(join(join("RCPT TO:","<"),receiver),">"), end_command);
    testPrint(Rcpt_message);
    send(s_fd, Rcpt_message, strlen(Rcpt_message), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);

    // TODO: Send DATA command and print server response
    const char * Data = join("data", end_command);
    testPrint(Data);
    send(s_fd, Data, strlen(Data), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    
    
    
    
    
    char Message_body[100];
    FILE* f_message = fopen(msg, "rt");
    FILE * f_application = fopen(att_path,"rt");
    FILE* Base64_file = fopen("Base64file", "w+");
    encode_file(f_application, Base64_file);
    fclose(Base64_file);
    fclose(f_application);
    Base64_file = fopen("Base64file", "r");
    fseek(Base64_file, 0, SEEK_END); 
    int fileSize;
    fileSize = ftell(Base64_file);                                       //获取fp的位置
    printf("fileSize=====%d\n", fileSize);
    char *Application_body;
    Application_body = (char*)malloc(fileSize + 1);
    rewind(Base64_file);
    fread(Application_body, sizeof(char),fileSize, Base64_file);
    Application_body[fileSize] = '\0';
    printf("strText=====%s\n", Application_body);
    fclose(Base64_file);

    fgets(Message_body, 99, f_message);
    fclose(f_message);


    const char * Message_title = join(join("From: ", user),"\r\n");
    testPrint(Message_title);
    send(s_fd, Message_title, strlen(Message_title), 0);

    Message_title = join(join("To: ", receiver),"\r\n");
    testPrint(Message_title);
    send(s_fd, Message_title, strlen(Message_title), 0);

    Message_title = join("MIME-Version: 1.0", "\r\n");
    testPrint(Message_title);
    send(s_fd, Message_title, strlen(Message_title), 0);

    Message_title = join("Content-Type: multipart/mixed; boundary=wzyyydswzyyydswzyyyds", "\r\n");
    testPrint(Message_title);
    send(s_fd, Message_title, strlen(Message_title), 0);

    Message_title = join(join(join("subject: ", subject), end_command), end_command);
    testPrint(Message_title);
    send(s_fd, Message_title, strlen(Message_title), 0);

    testPrint(boundary);
    send(s_fd, boundary, strlen(boundary),0);

    const char * encodeLabel = "Content-Transfer-Encoding: base64\r\n";
    testPrint(encodeLabel);
    send(s_fd, encodeLabel, strlen(encodeLabel), 0);

    const char * Application_head = join(join("Content-Type: application/zip; name=wzyyyds.zip", end_command) , end_command);
    testPrint(Application_head);
    send(s_fd, Application_head, strlen(Application_head), 0);
    
    const char * Ab = join(join(Application_body, end_command), end_command);
    testPrint(Ab);
    send(s_fd, Ab, strlen(Ab),0);
    

    testPrint(boundary);
    send(s_fd, boundary, strlen(boundary),0);

    

    const char * Text_head = join(join("Content-Type: text/plain", end_command), end_command);
    testPrint(Text_head);
    send(s_fd, Text_head, strlen(Text_head), 0);
    
    
    
    const char * Mb = join(join(Message_body, end_command), end_command);
    testPrint(Mb);
    send(s_fd, Mb, strlen(Mb),0);
    
    testPrint(boundary);
    send(s_fd, boundary, strlen(boundary),0);

    send(s_fd, end_msg, strlen(end_msg), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator

    // TODO: Send QUIT command and print server response
    const char * Quit = join("quit", end_command);
    testPrint(Quit);
    send(s_fd, Quit, strlen(Quit), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    close(s_fd);
    
}

int main(int argc, char* argv[])
{
    int opt;
    char* s_arg = NULL;
    char* m_arg = NULL;
    char* a_arg = NULL;
    char* recipient = NULL;
    const char* optstring = ":s:m:a:";
    while ((opt = getopt(argc, argv, optstring)) != -1)
    {
        switch (opt)
        {
        case 's':
            s_arg = optarg;
            break;
        case 'm':
            m_arg = optarg;
            break;
        case 'a':
            a_arg = optarg;
            break;
        case ':':
            fprintf(stderr, "Option %c needs an argument.\n", optopt);
            exit(EXIT_FAILURE);
        case '?':
            fprintf(stderr, "Unknown option: %c.\n", optopt);
            exit(EXIT_FAILURE);
        default:
            fprintf(stderr, "Unknown error.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc)
    {
        fprintf(stderr, "Recipient not specified.\n");
        exit(EXIT_FAILURE);
    }
    else if (optind < argc - 1)
    {
        fprintf(stderr, "Too many arguments.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        recipient = argv[optind];
        send_mail(recipient, s_arg, m_arg, a_arg);
        exit(0);
    }
}
