using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Mail;

namespace EmailHandler
{

    struct SSL_CREDENTIALS
    {
       public string username;
       public string password;
    }
    struct MAIL_INFO
    {
        public bool ssl;
        public String mailServer;
        public String subject;
        public String body;
        public String fromAddr;
        public String toAddr;
        public String attachment;
        public SSL_CREDENTIALS sslCredentials;      
    }
   
    class cSmtpEmail
    {       
       public String SendEmail
       (
          MAIL_INFO mailInfo
       )
       {
           int port;

           if(mailInfo.ssl)           
              port = 587;
           else
              port = 25;
           try
           {
               MailMessage mail = new MailMessage();
               SmtpClient SmtpServer = new SmtpClient(mailInfo.mailServer);

               mail.From = new MailAddress(mailInfo.fromAddr);
               mail.To.Add(mailInfo.toAddr);
               mail.Subject = mailInfo.subject;
               mail.Body = mailInfo.body;

               try
               {
                   System.Net.Mail.Attachment attachment;
                   attachment = new System.Net.Mail.Attachment(mailInfo.attachment);
                   mail.Attachments.Add(attachment);
               }
               catch (Exception)
               {
                  // invalid or no file attachment, continue with 
                  // the email
               }

             
               SmtpServer.Port = port;
               SmtpServer.Credentials = new System.Net.NetworkCredential(mailInfo.sslCredentials.username, mailInfo.sslCredentials.password );
               SmtpServer.EnableSsl = mailInfo.ssl;

               SmtpServer.Send(mail);
               return "mail sent\nServer: " + mailInfo.mailServer;
           }
           catch (Exception ex)
           {  
               return ex.ToString();
           }
       }
    }
}


