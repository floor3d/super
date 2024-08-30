<script>
  let csrf = document.getElementsByName("csrf-token")[0]?.content;

  const whoami = () => {
    fetch("/api/data", {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        "X-CSRFToken": csrf,
      },
      credentials: "same-origin",
    })
    .then((res) => res.json())
    .then((data) => {
      alert(`Welcome, ${data.username}!`);
    })
    .catch((err) => {
      console.log(err);
    });
  };

  const logout = () => {
    fetch("/api/logout", {
      credentials: "same-origin",
    })
    .then(() => {
      navigate("/login");
    })
    .catch((err) => {
      console.log(err);
    });
  };
</script>

<div>
  <h1>You are authenticated!</h1>
  <button type="button" on:click={whoami}>whoami</button>
  <button type="button" on:click={logout}>logout</button>
</div>

