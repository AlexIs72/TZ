using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using TranslatorApp.Models;

namespace TranslatorApp.Data
{
    public class LangDbContext : DbContext
    {
        public LangDbContext() { }
        public LangDbContext(DbContextOptions<LangDbContext> options) : base(options) { }

        protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
        {
            //Set the filename of the database to be created
            optionsBuilder.UseSqlite("Data Source=translator.db");
        }

        public DbSet<Lang> Langs { get; set; }
        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            modelBuilder.Entity<Lang>(b =>
                {
                    b.HasKey(e => e.ID);
                    b.Property(e => e.ID).IsRequired().HasMaxLength(2);
                    b.Property(e => e.Title).IsRequired().HasMaxLength(64);
                    b.ToTable("langs");
                }
            );
        }
    }
}
