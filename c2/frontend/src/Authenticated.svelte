<script>
  import { onMount } from "svelte";
  import { isAuthenticated, user, checkAuth, logout } from "./authStore";
  import { navigate } from "svelte-routing";

  let implants = [];

let error = null;

onMount(async () => {
  try {
    await checkAuth();

    if ($isAuthenticated && $user) {
      const res = await fetch("http://localhost:5000/api/implants", {
        credentials: "include",
      });
      implants = await res.json();
    } else {
      navigate("/login");
    }
  } catch (err) {
    console.error(err);
    error = 'Failed to load data.';
  }
});
</script>

{#if $isAuthenticated && $user}
  <h1>Welcome, {$user.username}!</h1>
  <!-- Display implants data -->
  <table>
    <thead>
      <tr>
        <th>ID</th>
        <th>First Connected</th>
        <th>Is Active</th>
      </tr>
    </thead>
    <tbody>
      {#each implants as implant}
        <tr>
          <td>{implant.id}</td>
          <td>{implant.firstconnected}</td>
          <td>{implant.isactive}</td>
          <td><a href="/implant?implantId={implant.id}">X</a></td>
        </tr>
      {/each}
    </tbody>
  </table>
  <button on:click={logout}>Logout</button>
{:else}
  <!-- Optionally, you can display a loading indicator or redirect -->
  <p>Loading...</p>
{/if}
