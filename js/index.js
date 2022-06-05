var TOP = 56;

/* Scroll menu */
$(window).on("scroll", function (event) {
  var scroll = $(window).scrollTop();
  var sections = [];
  $("*[id^=link-]").each(function (index, element) {
    var id = element.id.slice(5);
    var dest = document.getElementById(id);
    sections.push({
      id: id,
      end: dest.offsetTop + dest.offsetHeight,
    });
  });
  sections.sort(function (a, b) {
    return a.end - b.end;
  });
  console.log(sections);
  sections.every(function (div) {
    if (scroll + TOP < div.end) {
      $(".active").removeClass("active");
      $("#link-" + div.id).addClass("active");
      history.pushState(null, null, "index.html#" + div.id);
      return false;
    } else {
      return true;
    }
  });
});
