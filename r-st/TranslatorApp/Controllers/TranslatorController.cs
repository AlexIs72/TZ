using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using Microsoft.AspNetCore.Mvc;
using TranslatorApp.Data;

namespace TranslatorApp.Controllers
{
    public class PostData
    {
        public string src_text { get; set; }
        public string dst_text { get; set; }
        public string src_lang { get; set; }
        public string dst_lang { get; set; }
    }
    
    public class TranslatorController : Controller
    {
        public IActionResult Index()
        {
            var db = new LangDbContext();
            ViewBag.langs = db.Langs.ToList();
            return View();
        }

        [HttpGet("/Translator/DownloadXml")]
        public FileStreamResult DownloadXml()
        {
            var stream = new MemoryStream();
            var writer = new StreamWriter(stream);
            byte[] data = { };

            if(HttpContext.Session.TryGetValue("translated_data", out data))
            {
                writer.Write(Encoding.UTF8.GetString(data));
                writer.Flush();
                stream.Position = 0;
            }

            return File(stream, "text/plain", "translation_result.txt");
        }


        [HttpPost("/Translator/GenerateXml")]
        public ActionResult GenerateXml([FromBody] object data)
        {
            PostData post_data = Newtonsoft.Json.JsonConvert.DeserializeObject<PostData>(data.ToString());

            XElement translate = new XElement("translate",
                    new XElement("rawText", new XAttribute("code", post_data.src_lang), new XAttribute("value", post_data.src_text)),
                    new XElement("translateText", new XAttribute("code", post_data.dst_lang), new XAttribute("value", post_data.dst_text)),
                    new XElement("time", new XAttribute("value", DateTime.Now))
                );


            HttpContext.Session.Set("translated_data", Encoding.UTF8.GetBytes(translate.ToString()));

            return new EmptyResult();
        }
    }
}