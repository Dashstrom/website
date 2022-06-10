var TOP = 56;

var progressBar = document.getElementById("progress");

/* Scroll with right item menu */
$(window).on("scroll", function (event) {
  var scroll = $(window).scrollTop();
  var sections = [];
  $("*[id^=link-]").each(function (index, element) {
    var id = element.id.slice(5);
    var dest = document.getElementById(id);
    if (!dest) return;
    sections.push({
      id: id,
      end: dest.offsetTop + dest.offsetHeight,
    });
  });
  sections.sort(function (a, b) {
    return a.end - b.end;
  });
  var id = null;

  $(".active").removeClass("active");
  sections.every(function (part) {
    if (scroll + TOP < part.end) {
      id = part.id;
      return false;
    } else {
      return true;
    }
  });
  if (id === null && sections.length > 0) {
    id = sections[0].id;
  }
  if (id != null) {
    $("#link-" + id).addClass("active");
    history.pushState(null, null, "index.html#" + id);
  }

  var winScroll = document.body.scrollTop || document.documentElement.scrollTop;
  var height = document.documentElement.scrollHeight - document.documentElement.clientHeight;
  var scrolled = (winScroll / height) * 100;
  progressBar.style.width = scrolled + "%";
  
});

/* Close menu on click */
$(".navbar-nav>li>a").on("click", function () {
  $(".navbar-collapse").collapse("hide");
});

