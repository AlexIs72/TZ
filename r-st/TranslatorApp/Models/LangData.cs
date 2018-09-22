using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using TranslatorApp.Data;

namespace TranslatorApp.Models
{
    public class LangData
    {
        public static void Initialize(IServiceProvider serviceProvider)
        {
            using (var context = new LangDbContext(
                serviceProvider.GetRequiredService<DbContextOptions<LangDbContext>>()))
            {
                // Look for any movies.
                if (context.Langs.Any())
                {
                    return;   // DB has been seeded
                }

                context.Langs.AddRange(
                    new Lang
                    {
                        ID = "ru",
                        Title = "Русский",
                    },
                    new Lang
                    {
                        ID = "en",
                        Title = "Английский",
                    },
                    new Lang
                    {
                        ID = "sl",
                        Title = "Словенский",
                    }
                );
                context.SaveChanges();
            }
        }
    }
}
